// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AOGameplayAbility.h"
#include "Attributes/AOAttributeSet.h"

FString UAOGameplayAbility::GetDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"),
	L"Default Ability Name - LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum", Level);
}

FString UAOGameplayAbility::GetNextLevelDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Next Level: </><Level>%d</> \n<Default>Causes much more damage. </>"), Level);
}

FString UAOGameplayAbility::GetLockedDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Ability Locked Until Level: %d</>"), Level);
}

float UAOGameplayAbility::GetEnergyCost(float InLevel /*= 1.f*/) const
{	
	float EnergyCost = 0.f;
	if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		for (auto Mod : CostEffect->Modifiers)
		{
			if (Mod.Attribute == UAOAttributeSet::GetEnergyAttribute())
			{
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, EnergyCost);
				break;
			}
		}
	}
	return EnergyCost;
}
float UAOGameplayAbility::GetCooldown(float InLevel /*= 1.f*/) const
{
	float Cooldown = 0.f;
	if (const UGameplayEffect* CooldownEffect = GetCostGameplayEffect())
	{
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
	}
	return Cooldown;
}
