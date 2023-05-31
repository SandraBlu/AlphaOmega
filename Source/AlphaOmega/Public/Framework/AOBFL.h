// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AOBFL.generated.h"

class UAOCoreComponent;

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UAOBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "Components")
	static UAOCoreComponent* GetAOCoreComponent(const AActor* Actor);

};
