// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "XPInfo.generated.h"

USTRUCT(BlueprintType)
struct FLevelUpInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 LevelUpPointsRequired = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AttributePointsRewarded = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AbilityPointsRewarded = 1.f;

};
/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UXPInfo : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TArray<FLevelUpInfo> LevelUpInfo;

	int32 FindLevelForXP(int32 XP) const;
	
};
