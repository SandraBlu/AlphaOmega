// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AODamageEffectCalc.generated.h"

/**
 * Damage exec that combines raw damage with attack/defense power
 */
UCLASS()
class ALPHAOMEGA_API UAODamageEffectCalc : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:

	UAODamageEffectCalc();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecuteParams, OUT FGameplayEffectCustomExecutionOutput& OutExecuteOutput) const override;
};
