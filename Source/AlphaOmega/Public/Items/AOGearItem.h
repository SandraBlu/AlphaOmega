// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/AOEquipItem.h"
#include "AOGearItem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ALPHAOMEGA_API UAOGearItem : public UAOEquipItem
{
	GENERATED_BODY()
	
		public:

	UAOGearItem();

	virtual bool Equip(class AAOPlayer* Character) override;
	virtual bool Unequip(class AAOPlayer* Character) override;
	

	//Skeletal Mesh for Gear
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gear")
	class USkeletalMesh* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gear")
	class UMaterialInstance* MaterialInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gear")
	float DamageDefenseMult;
};
