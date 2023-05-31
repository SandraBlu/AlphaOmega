// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/AOItem.h"
#include "AOEquipItem.generated.h"

//AllSlotsfor gear
UENUM(BlueprintType)
enum class EEquipSlot : uint8
{
	EIS_Head UMETA(DisplayName = "Head"),
	EIS_HeadAccessory UMETA(DisplayName = "HeadAccessory"),
	EIS_Torso UMETA(DisplayName = "Torso"),
	EIS_Legs UMETA(DisplayName = "Legs"),
	EIS_Hands UMETA(DisplayName = "Hands"),
	EIS_Feet UMETA(DisplayName = "Feet"),
	EIS_Backpack UMETA(DisplayName = "Backpack"),
	EIS_PrimaryWeapon UMETA(DisplayName = "Primary Weapon"),
	EIS_SecondaryWeapon UMETA(DisplayName = "Secondary Weapon"),
	EIS_Canteen UMETA(DisplayName = "Canteen")
};
/**
 * 
 */
UCLASS(Abstract, NotBlueprintable)
class ALPHAOMEGA_API UAOEquipItem : public UAOItem
{
	GENERATED_BODY()

public:

		UAOEquipItem();

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	EEquipSlot Slot;

	virtual void Use(class AAOPlayer* Character) override;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	virtual bool Equip(class AAOPlayer* Character);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	virtual bool Unequip(class AAOPlayer* Character);

	virtual bool ShouldShowInInventory() const override;
	virtual void AddedToInventory(class UAOInventoryComponent* Inventory) override;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	bool IsEquipped() { return bEquipped; };

	void SetEquipped(bool bNewEquipped);


protected:

	UPROPERTY()
	bool bEquipped;

	UFUNCTION()
	void EquipStatusChanged();
};
