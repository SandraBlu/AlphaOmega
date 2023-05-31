// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AOCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InteractComponent.h"
#include "Components/AOCoreComponent.h"
#include "Framework/AOBFL.h"
#include "Attributes/AOPlayerAttributeSet.h"



// Sets default values
AAOCharacter::AAOCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);

	AbilitySystemComp = CreateDefaultSubobject<UAbilitySystemComponent>("AbilityComp");
	Attributes = CreateDefaultSubobject<UAOPlayerAttributeSet>("Attributes");
}

UAbilitySystemComponent* AAOCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComp;
}

void AAOCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (AbilitySystemComp)
		AbilitySystemComp->InitAbilityActorInfo(this, this);

	InitializeAttributes();
	GrantAbilities();
}

void AAOCharacter::InitializeAttributes()
{
	if (AbilitySystemComp && GrantedEffect)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComp->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComp->MakeOutgoingSpec(GrantedEffect, 1, EffectContext);

		if (SpecHandle.IsValid())
			FActiveGameplayEffectHandle GEHandle = AbilitySystemComp->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AAOCharacter::GrantAbilities()
{
	if (AbilitySystemComp)
		for (TSubclassOf<UGameplayAbility>& StartupAbility : GrantedAbilities)
			AbilitySystemComp->GiveAbility(FGameplayAbilitySpec(StartupAbility.GetDefaultObject(), 1, 0));
}

void AAOCharacter::ChangeAbilityLevelWithTags(FGameplayTagContainer TagContainer, int32 NewLevel)
{
	TArray<FGameplayAbilitySpec*> MatchingAbility;
	AbilitySystemComp->GetActivatableGameplayAbilitySpecsByAllMatchingTags(TagContainer, MatchingAbility, true);
	for (FGameplayAbilitySpec* Spec : MatchingAbility)
	{
		Spec->Level = NewLevel;
	}
}

void AAOCharacter::ApplyGEtoTarget(const FGameplayEffectSpecHandle& GESpec, const FGameplayAbilityTargetDataHandle& TargetData)
{
	for (TSharedPtr<FGameplayAbilityTargetData> Data : TargetData.Data)
		{
			Data->ApplyGameplayEffectSpec(*GESpec.Data.Get());
		}
}

