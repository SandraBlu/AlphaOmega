// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AOCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AOFootstepsComponent.h"
#include "AbilitySystem/AOAbilityComp.h"
#include "AbilitySystemComponent.h"
#include "Weapons/AOWeapon.h"
#include "Components/CapsuleComponent.h"
#include "AOGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "FX/DebuffNiagaraComp.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AAOCharacter::AAOCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;

	//Debuff
	const FAOGameplayTags& GameplayTags = FAOGameplayTags::Get();
	NiagaraDebuffComp = CreateDefaultSubobject<UDebuffNiagaraComp>("BurnDebuff");
	NiagaraDebuffComp->SetupAttachment(GetRootComponent());
	NiagaraDebuffComp->DebuffTag = GameplayTags.Debuff_Burn;

	// Enabled on mesh to react to incoming projectiles
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	//GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);

	FootstepsComp = CreateDefaultSubobject<UAOFootstepsComponent>("FootstepsComp");
}

UAbilitySystemComponent* AAOCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UNiagaraSystem* AAOCharacter::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

FTaggedMontage AAOCharacter::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (FTaggedMontage TaggedMontage : AttackMontages)
	{
		if (TaggedMontage.MontageTag == MontageTag)
		{
			return TaggedMontage;
		}
	}
	return FTaggedMontage();
}

ECharacterClass AAOCharacter::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

FOnDeath AAOCharacter::GetOnDeathDelegate()
{
	return OnDeath;
}

void AAOCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAOCharacter, bIsStunned);
	DOREPLIFETIME(AAOCharacter, bIsBurned);
	DOREPLIFETIME(AAOCharacter, bIsBeingShocked);
}

UAOFootstepsComponent* AAOCharacter::GetFootstepsComp() const
{
	return FootstepsComp;
}

UAnimMontage* AAOCharacter::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void AAOCharacter::Die()
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSFX, GetActorLocation(), GetActorRotation());
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bDead = true;
	OnDeath.Broadcast(this);
}


bool AAOCharacter::IsDead_Implementation() const
{
	return bDead;
}

AActor* AAOCharacter::GetAvatar_Implementation()
{
	return this;
}

TArray<FTaggedMontage> AAOCharacter::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

void AAOCharacter::OnRep_Stunned()
{
}

void AAOCharacter::OnRep_Burned()
{
}

void AAOCharacter::InitAbilityActorInfo()
{

}

void AAOCharacter::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass, float Level) const
{
	if (GetAbilitySystemComponent() && EffectClass)
	{
		FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponent()->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(EffectClass, Level, EffectContext);

		GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
	}
}

void AAOCharacter::InitializeAttributes() const
{
	ApplyEffectToSelf(PrimaryAttributes, 1.f);
	ApplyEffectToSelf(SecondaryAttributes, 1.f);
	ApplyEffectToSelf(BaseAttributes, 1.f);
	ApplyEffectToSelf(ResistanceAttributes, 1.f);
}

void AAOCharacter::GrantAbilities()
{
	GetASC()->AddGrantedAbilities(GrantedAbilities);
	GetASC()->AddPassiveAbilities(PassiveAbilities);
}

FVector AAOCharacter::GetCombatSocketLocation_Implementation(const FGameplayTag& CombatSocketTag)
{
	return FVector();
}

void AAOCharacter::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bIsStunned = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bIsStunned ? 0.f : BaseWalkSpeed;
}

UAOAbilityComp* AAOCharacter::GetASC()
{
	if (AOASC == nullptr)
	{
		AOASC = CastChecked<UAOAbilityComp>(AbilitySystemComponent);
	}
	return AOASC;
}
