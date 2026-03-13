// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"

#include "GAS/OAbilitySystemComponent.h"


// Sets default values
ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	/*EffectDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("EffectDebuffComponent");
	EffectDebuffComponent->SetupAttachment(GetRootComponent());
	EffectDebuffComponent->DebuffTag = FRGameplayTags::Get().Debuff_Stun;

	EffectAttachComp = CreateDefaultSubobject<USceneComponent>("EffectAttachPoint");
	EffectAttachComp->SetupAttachment(GetRootComponent());
	PassiveCloak = CreateDefaultSubobject<UPassiveNiagaraComponent>("CloakComponent");
	PassiveCloak->SetupAttachment(EffectAttachComp);
	PassiveLifeSiphon = CreateDefaultSubobject<UPassiveNiagaraComponent>("LifeSiphonComponent");
	PassiveLifeSiphon->SetupAttachment(EffectAttachComp);
	PassiveStaminaSiphon = CreateDefaultSubobject<UPassiveNiagaraComponent>("StaminaSiphonComponent");
	PassiveStaminaSiphon->SetupAttachment(EffectAttachComp);*/

	//MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::InitAbilityActorInfo()
{
}

void ABaseCharacter::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void ABaseCharacter::InitializeAttributes() const
{
	ApplyEffectToSelf(PrimaryAttributes, 1.f);
	ApplyEffectToSelf(SecondaryAttributes, 1.f);
	ApplyEffectToSelf(ResistanceAttributes, 1.f);
	ApplyEffectToSelf(VitalAttributes, 1.f);
}

void ABaseCharacter::GrantAbilities()
{
	UOAbilitySystemComponent* ASC = CastChecked<UOAbilitySystemComponent>(AbilitySystemComponent);
	if (!HasAuthority()) return;
	ASC->AddGrantedAbilities(GrantedAbilities);
	ASC->AddPassiveAbilities(GrantedPassiveAbilities);
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//EffectAttachComp->SetWorldRotation((FRotator::ZeroRotator));

}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,AActor* DamageCauser)
{
	float DamageTaken =  Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	OnDamageDelegate.Broadcast(DamageTaken);
	return DamageTaken;
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UCombatComponent* ABaseCharacter::GetCombatComponent() const
{
	return CombatComp;
}

UAnimMontage* ABaseCharacter::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

UAnimMontage* ABaseCharacter::GetStunnedMontage_Implementation()
{
	return StunnedMontage;
}

bool ABaseCharacter::IsDead_Implementation() const
{
	return bDead;
}

AActor* ABaseCharacter::GetAvatar_Implementation()
{
	return this;
}

TArray<FTaggedMontage> ABaseCharacter::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

UNiagaraSystem* ABaseCharacter::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

ECharacterClass ABaseCharacter::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

FOnASCRegistered ABaseCharacter::GetOnASCRegisteredDelegate()
{
	return OnASCRegistered;
}

FOnDeath ABaseCharacter::GetOnDeathDelegate()
{
	return OnDeath;
}

void ABaseCharacter::Die(const FVector& DeathImpulse)
{
	
}

FOnDamageSignature& ABaseCharacter::GetOnDamageSignature()
{
	return OnDamageDelegate;
}