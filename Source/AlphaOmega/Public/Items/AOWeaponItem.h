// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/AOEquipItem.h"
#include "AOWeaponItem.generated.h"

class AAOPlayer;
class AAOWeapon;

/**
 * 
 */
UCLASS(Blueprintable)
class ALPHAOMEGA_API UAOWeaponItem : public UAOEquipItem
{
	GENERATED_BODY()
	
		public:

	UAOWeaponItem();

	virtual bool Equip(class AAOPlayer* Character) override;
	virtual bool Unequip(class AAOPlayer* Character) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AAOWeapon> WeaponClass;
};
