// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MissileAbility.h"
#include "ElectricMissile.generated.h"

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UElectricMissile : public UMissileAbility
{
	GENERATED_BODY()

public:

	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
	
};
