// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEC_Damage.generated.h"

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UGEC_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UGEC_Damage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	void DetermineDebuff(const FGameplayEffectSpec& Spec, const FGameplayEffectCustomExecutionParameters& ExecutionParams, FAggregatorEvaluateParameters& EvalParameters, const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& InTagToDef) const;

	
};
