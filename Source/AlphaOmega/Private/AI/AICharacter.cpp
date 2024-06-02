// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AICharacter.h"
#include "AlphaOmega/AlphaOmega.h"
#include "Perception/PawnSensingComponent.h"
#include "AI/AOAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystem/AOAbilityComp.h"
#include "Attributes//AOAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "UI/AOUserWidget.h"
#include "Framework/AOBFL.h"
#include "AOGameplayTags.h"

#define LOCTEXT_NAMESPACE "LootAI"

AAICharacter::AAICharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAOAbilityComp>("ASC");
	AttributeSet = CreateDefaultSubobject<UAOAttributeSet>("Attributes");

	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	Health = CreateDefaultSubobject<UWidgetComponent>("Health");
	Health->SetupAttachment(GetRootComponent());

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

 	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
 	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
 	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Combat");
	Weapon->SetupAttachment(GetMesh(), FName("weapon"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TimeToHitParamName = "TimeToHit";
	TargetActorKey = "TargetActor";
}

void AAICharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitAbilityActorInfo();
	UAOBFL::GiveNPCAbilities(this, AbilitySystemComponent, CharacterClass);


	//Set widget controller for enemy health bar
	if (UAOUserWidget* EnemyHealthUI = Cast<UAOUserWidget>(Health->GetUserWidgetObject()))
	{
		EnemyHealthUI->SetWidgetController(this);
	}
	//Set up binding value changes (lambdas) for Progress bar
	if (const UAOAttributeSet* AS = Cast<UAOAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChange.Broadcast(Data.NewValue);
			}
		);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChange.Broadcast(Data.NewValue);
			}
		);
		//Hit react Event Tag to GE_HitReact
		AbilitySystemComponent->RegisterGameplayTagEvent(FAOGameplayTags::Get().ability_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAICharacter::HitReactTagChanged);


		//Set Initial values for attributes
		OnHealthChange.Broadcast(AS->GetHealth());
		OnMaxHealthChange.Broadcast(AS->GetMaxHealth());
	}
}

void AAICharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AIC = Cast<AAOAIController>(NewController);
	AIC->GetBlackboardComponent()->InitializeBlackboard(*BTree->BlackboardAsset);
	AIC->RunBehaviorTree(BTree);
	AIC->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	AIC->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass != ECharacterClass::Warrior);
}
int32 AAICharacter::GetCharacterLevel_Implementation()
{
	return Level;
}

void AAICharacter::Die()
{
	SetLifeSpan(LifeSpan);
	if (AIC) AIC->GetBlackboardComponent()->SetValueAsBool(FName("IsDead"), true);
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	Super::Die();

	DissolveMesh();
}

void AAICharacter::GetHit_Implementation(const FVector& ImpactPoint)
{
	DrawDebugSphere(GetWorld(), ImpactPoint, 10.f, 12, FColor::Cyan, false, 5.f);

	DirectionalHitReact(ImpactPoint);

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
	if (HitParticles)
	{
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitParticles, ImpactPoint);
	}
}

void AAICharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	//Lower Impact Point to Enemy's .Z location
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector HitDist = (ImpactLowered - GetActorLocation()).GetSafeNormal();


	//Find Angle
	//(Forward(1) * HitDist(1)) * cos(theta) = DOt Product
	const double CosTheta = FVector::DotProduct(Forward, HitDist);
	//Take inverse cosine (arc-cosine)  of cos (theta) to get Theta
	double Theta = FMath::Acos(CosTheta);
	//convert from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);

	const FVector CrossProduct = FVector::CrossProduct(Forward, HitDist);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	FName Section("HitB");

	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("HitF");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("HitL");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("HitR");
	}

	PlayHitReactMontage(Section);

}

void AAICharacter::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	AIC->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
}



AActor* AAICharacter::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

void AAICharacter::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

void AAICharacter::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAOAbilityComp>(AbilitySystemComponent)->AbilityActorInfoInit();

	InitializeAttributes();
}

void AAICharacter::InitializeAttributes() const
{
	UAOBFL::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

FVector AAICharacter::GetCombatSocketLocation_Implementation(const FGameplayTag& CombatSocketTag)
{
	const FAOGameplayTags& GameplayTags = FAOGameplayTags::Get();
	if (CombatSocketTag.MatchesTagExact(GameplayTags.combatSocket_weapon) && IsValid(Weapon))
	{
		return Weapon->GetSocketLocation(DamageSocket);
	}
	if (CombatSocketTag.MatchesTagExact(GameplayTags.combatSocket_handL))
	{
		return GetMesh()->GetSocketLocation(LHand);
	}
	if (CombatSocketTag.MatchesTagExact(GameplayTags.combatSocket_handR))
	{
		return GetMesh()->GetSocketLocation(RHand);
	}
	return FVector();
}

void AAICharacter::SetTargetActor(AActor* NewTarget)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		AIController->GetBlackboardComponent()->SetValueAsObject("TargetActor", NewTarget);
	}
}

void AAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AAICharacter::OnPawnSeen);
}

void AAICharacter::OnPawnSeen(APawn* Pawn)
{
	SetTargetActor(Pawn);

	DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::Green, 4.f, true);
}

void AAICharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReact)
	{
		AnimInstance->Montage_Play(HitReact);
		AnimInstance->Montage_JumpToSection(SectionName, HitReact);
	}
}

#undef LOCTEXT_NAMESPACE