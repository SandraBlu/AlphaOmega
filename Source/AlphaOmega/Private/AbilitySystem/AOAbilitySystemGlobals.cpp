// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AOAbilitySystemGlobals.h"
#include "AOGameplayEffectTypes.h"

FGameplayEffectContext* UAOAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FAOGameplayEffectContext();
}
