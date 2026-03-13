// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/OGameplayAbility.h"

#include "AI/NPCBase.h"
#include "Character/PlayerCharacter.h"
#include "Framework/OPlayerController.h"
#include "GAS/OAbilitySystemComponent.h"
#include "GAS/OAttributeSet.h"

FString UOGameplayAbility::GetDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"),
	L"Default Ability Name - LoremIpsum ", Level);
}

FString UOGameplayAbility::GetNextLevelDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Next Level: </><Level>%d</> \n<Default>Causes much more damage. </>"), Level);
}

FString UOGameplayAbility::GetLockedDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Ability Locked Until Level: %d</>"), Level);
}

float UOGameplayAbility::GetEnergyCost(float InLevel) const
{
	float EnergyCost = 0.f;
	if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		for (FGameplayModifierInfo Mod : CostEffect->Modifiers)
		{
			if (Mod.Attribute == UOAttributeSet::GetEnergyAttribute())
			{
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, EnergyCost);
				break;
			}
		}
	}
	return EnergyCost;
}

float UOGameplayAbility::GetCooldown(float InLevel) const
{
	float Cooldown = 0.f;
	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
	}
	return Cooldown;
}

UOAbilitySystemComponent* UOGameplayAbility::GetRAbilitySystemComponentFromActorInfo() const
{
	return Cast<UOAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}

APlayerCharacter* UOGameplayAbility::GetPlayerFromActorInfo()
{
	if (!CachedPlayer.IsValid())
	{
		CachedPlayer = Cast<APlayerCharacter>(CurrentActorInfo->AvatarActor);
	}
   
	return CachedPlayer.IsValid()? CachedPlayer.Get() : nullptr;
}

class AOPlayerController* UOGameplayAbility::GetPlayerControllerFromActorInfo()
{
	if (!CachedPlayerController.IsValid())
	{
		CachedPlayerController = Cast<AOPlayerController>(CurrentActorInfo->PlayerController);
	}

	return CachedPlayerController.IsValid()? CachedPlayerController.Get() : nullptr;
}

class ANPCBase* UOGameplayAbility::GetEnemyCharacterFromActorInfo()
{
	if (!CachedEnemy.IsValid())
	{
		CachedEnemy = Cast<ANPCBase>(CurrentActorInfo->AvatarActor);
	}
	return CachedEnemy.IsValid()? CachedEnemy.Get() : nullptr;
}