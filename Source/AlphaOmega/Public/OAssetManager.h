// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "OAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UOAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:

	static UOAssetManager& Get();

protected:

	virtual void StartInitialLoading() override;
	
};
