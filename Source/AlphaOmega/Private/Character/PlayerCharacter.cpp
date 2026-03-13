// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraComponent.h"
#include "OGameplayTags.h"
#include "Components/CapsuleComponent.h"
#include "Components/FootstepComponent.h"
#include "Framework/OPlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/OAbilitySystemComponent.h"
#include "GAS/Data/LevelUpInfo.h"
#include "Input/OInputComponent.h"
#include "Inventory/EquipmentComponent.h"
#include "UI/OHUD.h"
#include "DebugHelper.h"
#include "Actor/Weapon.h"
#include "Kismet/GameplayStatics.h"

class UEnhancedInputLocalPlayerSubsystem;

APlayerCharacter::APlayerCharacter()
{
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	
	Gear = CreateDefaultSubobject<UEquipmentComponent>("GearComp");
	CharacterClass = ECharacterClass::Elementalist;

	LevelUpFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFXComp"));
	LevelUpFX->SetupAttachment(GetRootComponent());
	LevelUpFX->bAutoActivate = false;
	
	FootstepComponent = CreateDefaultSubobject<UFootstepComponent>(TEXT("FootstepComp"));

	bUsingBlock = false;
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	//server
	Super::PossessedBy(NewController);
	InitAbilityActorInfo();
	GrantAbilities();
}

void APlayerCharacter::OnRep_PlayerState()
{
	//client
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
}

int32 APlayerCharacter::GetPlayerLevel_Implementation()
{
	AOPlayerState* PS = GetPlayerState<AOPlayerState>();
	check(PS);
	return PS->GetPlayerLevel();
}

FVector APlayerCharacter::GetCombatSocketLocation_Implementation(const FGameplayTag& CombatSocketTag)
{
	const FOGameplayTags& GameplayTags = FOGameplayTags::Get();
	if (CombatSocketTag.MatchesTagExact(GameplayTags.combatSocket_weapon) && IsValid(Gear->EquippedWeapon))
	{
		return Gear->EquippedWeapon->GetWeaponMesh()->GetSocketLocation(Gear->EquippedWeapon->FiringSocket);
	}
	if (CombatSocketTag.MatchesTagExact(GameplayTags.combatSocket_handL))
	{
		return GetMesh()->GetSocketLocation(HandRSocket);
	}
	if (CombatSocketTag.MatchesTagExact(GameplayTags.combatSocket_handR))
	{
		return GetMesh()->GetSocketLocation(HandLSocket);
	}
	return FVector();
}

AActor* APlayerCharacter::GetCurrentEquippedWeapon_Implementation(AWeapon* InWeapon)
{
	return nullptr; //Gear->EquippedWeapon;
}

void APlayerCharacter::Die(const FVector& DeathImpulse)
{
	if (!Gear->EquippedWeapon) return;
	Gear->EquippedWeapon->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	Gear->EquippedWeapon->GetWeaponMesh()->SetSimulatePhysics(true);
	Gear->EquippedWeapon->GetWeaponMesh()->SetEnableGravity(true);
	Gear->EquippedWeapon->GetWeaponMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Gear->EquippedWeapon->GetWeaponMesh()->AddImpulse(DeathImpulse);

	UGameplayStatics::PlaySoundAtLocation(this, DeathCry, GetActorLocation(), GetActorRotation());
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	bDead = true;
	OnDeath.Broadcast(this);
}

void APlayerCharacter::AddToXP_Implementation(int32 InXP)
{
	AOPlayerState* PS = GetPlayerState<AOPlayerState>();
	check(PS);
	PS->AddToXP(InXP);
}

void APlayerCharacter::LevelUp_Implementation()
{
	MulticastLevelUpVFX();
}

void APlayerCharacter::MulticastLevelUpVFX_Implementation()
{
	if (IsValid(LevelUpFX))
	{
		LevelUpFX->Activate(true);
	}
}

int32 APlayerCharacter::GetXP_Implementation() const
{
	AOPlayerState* PS = GetPlayerState<AOPlayerState>();
	check(PS);
	return PS->GetXP();
}

int32 APlayerCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	AOPlayerState* PS = GetPlayerState<AOPlayerState>();
	check(PS);
	return PS->LevelUpInfo->FindLevelForXP(InXP);
}

int32 APlayerCharacter::GetAttributePtsReward_Implementation(int32 Level) const
{
	AOPlayerState* PS = GetPlayerState<AOPlayerState>();
	check(PS);
	return PS->LevelUpInfo->LevelUpInfo[Level].AttributePointReward;
}

int32 APlayerCharacter::GetAbilityPtsReward_Implementation(int32 Level) const
{
	AOPlayerState* PS = GetPlayerState<AOPlayerState>();
	check(PS);
	return PS->LevelUpInfo->LevelUpInfo[Level].AbilityPointReward;
}

void APlayerCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	AOPlayerState* PS = GetPlayerState<AOPlayerState>();
	check(PS);
	PS->AddToLevel(InPlayerLevel);
	if (UOAbilitySystemComponent* RASC = Cast<UOAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		RASC->UpdateAbilityStatus(PS->GetPlayerLevel());
	}
}

void APlayerCharacter::AddToAttributePts_Implementation(int32 InAttributePoints)
{
	AOPlayerState* PS = GetPlayerState<AOPlayerState>();
	check(PS);
	PS->AddToAttributePts(InAttributePoints);
}


void APlayerCharacter::AddToAbilityPts_Implementation(int32 InAbilityPoints)
{
	AOPlayerState* PS = GetPlayerState<AOPlayerState>();
	check(PS);
	PS->AddToAbilityPts(InAbilityPoints);
}

int32 APlayerCharacter::GetAttributePoints_Implementation() const
{
	AOPlayerState* PS = GetPlayerState<AOPlayerState>();
	check(PS);
	return PS->GetAttributePts();
}

int32 APlayerCharacter::GetAbilityPoints_Implementation() const
{
	AOPlayerState* PS = GetPlayerState<AOPlayerState>();
	check(PS);
	return PS->GetAbilityPts();
}

AWeapon* APlayerCharacter::GetCurrentWeapon_Implementation()
{
	if (Gear->EquippedWeapon)
	{
		return Gear->EquippedWeapon;
	}
	return nullptr;
}

UFootstepComponent* APlayerCharacter::GetFootstepsComp() const
{
	return FootstepComponent;
}

bool APlayerCharacter::GetIsUsingBlock()
{
	return bUsingBlock;
}

bool APlayerCharacter::SetIsUsingBlock(bool block)
{
	bUsingBlock = block;
	return bUsingBlock;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}
}

void APlayerCharacter::Input_SwitchTargetTriggered(const FInputActionValue& Value)
{
	SwitchTargetDirection = Value.Get<FVector2D>();
}

void APlayerCharacter::Input_SwitchTargetCompleted(const FInputActionValue& Value)
{
	FGameplayEventData EventData;
	const FOGameplayTags& GameplayTags = FOGameplayTags::Get();
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, SwitchTargetDirection.X > 0.f? GameplayTags.Event_SwitchTarget_Right : GameplayTags.Event_SwitchTarget_Left, EventData);
	Debug::Print((TEXT("Switch Target Direction: ") + SwitchTargetDirection.ToString()));
}

void APlayerCharacter::InitAbilityActorInfo()
{
	AOPlayerState* PS = GetPlayerState<AOPlayerState>();
	check(PS);
	PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
	Cast<UOAbilitySystemComponent>(PS->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	AttributeSet = PS->GetAttributeSet();
	OnASCRegistered.Broadcast(AbilitySystemComponent);
	
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (AOHUD* HUD = Cast<AOHUD>(PC->GetHUD()))
		{
			HUD->InitOverlay(PC, PS, AbilitySystemComponent, AttributeSet);
		}
	}
	InitializeAttributes();
}

void APlayerCharacter::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (Gear->EquippedWeapon == nullptr ) return;
	if (GetASC() == nullptr) return;
	if (GetASC()) GetASC()->AbilityInputTagPressed(InputTag);
}

void APlayerCharacter::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC() == nullptr) return;
	GetASC()->AbilityInputTagReleased(InputTag);
}

UOAbilitySystemComponent* APlayerCharacter::GetASC()
{
	if (RAbilitySystemComponent == nullptr)
	{
		RAbilitySystemComponent = CastChecked<UOAbilitySystemComponent>(AbilitySystemComponent);
	}
	return RAbilitySystemComponent;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UOInputComponent* InputComp = CastChecked<UOInputComponent>(InputComponent))
	{
		InputComp->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased);
		InputComp->BindAction(SwitchTarget, ETriggerEvent::Started, this, &APlayerCharacter::Input_SwitchTargetTriggered);
		InputComp->BindAction(SwitchTarget, ETriggerEvent::Completed, this, &APlayerCharacter::Input_SwitchTargetCompleted);
	}
}