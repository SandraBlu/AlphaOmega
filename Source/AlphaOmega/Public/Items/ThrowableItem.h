// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/AOEquipItem.h"
#include "ThrowableItem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ALPHAOMEGA_API UThrowableItem : public UAOEquipItem
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	class UAnimMontage* TossAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapons")
	TSubclassOf<class AThrowableWeapon> ThrowableClass;
	
};
