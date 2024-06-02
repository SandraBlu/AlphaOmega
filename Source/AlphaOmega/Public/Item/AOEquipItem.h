// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/AOItem.h"
#include "AOEquipItem.generated.h"

UENUM(BlueprintType)
enum class EEquipSlot : uint8 
{
	ES_Headwear UMETA(DisplayName = "Headwear"),
	ES_Torso UMETA(DisplayName = "Torso"),
	ES_Legs UMETA(DisplayName = "Legs"),
	ES_Feet UMETA(DisplayName = "Feet"),
	ES_Hands UMETA(DisplayName = "Hands"),
	ES_Backpack UMETA(DisplayName = "Backpack"),
	ES_Weapon UMETA(DisplayName = "Weapon"),
	ES_Sheath UMETA(DisplayName = "Sheath"),
	ES_Throwable UMETA(DisplayName = "Throwable")
	
};
/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UAOEquipItem : public UAOItem
{
	GENERATED_BODY()

public:
	
	friend class UAOEquipmentComponent;

	UAOEquipItem();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	EEquipSlot Slot;

	/**Allows you to override what equipping the item does. By default this sets the players mesh to the Equippable Mesh, but perhaps
you want a weapon actor to spawn in, or have an equippable aura effect that follows the player. by overriding HandleEquip you can 
do whatever custom logic you want. */
	UFUNCTION(BlueprintNativeEvent, Category = "Equippable")
	void HandleEquip();
	virtual void HandleEquip_Implementation();

	/**Allows you to override what happens when the item unequips. */
	UFUNCTION(BlueprintNativeEvent, Category = "Equippable")
	void HandleUnequip();
	virtual void HandleUnequip_Implementation();

	virtual void Activated_Implementation() override;
	
	virtual void Deactivated_Implementation() override;
	
};

/**
 * Defines a clothing item. Will use the skeletal mesh set in your EquipmentComponent and set it to the clothing mesh you select.
 */
UCLASS()
class ALPHAOMEGA_API UAOEquipItem_Clothing : public UAOEquipItem
{
	GENERATED_BODY()

public:

	UAOEquipItem_Clothing();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif 

	virtual void HandleEquip_Implementation() override;
	virtual void HandleUnequip_Implementation() override;

	/**The mesh to put on the player when they equip the item*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equippable")
	class USkeletalMesh* ClothingMesh;

	/**The materials to apply to the clothing mesh*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equippable")
	TArray<class UMaterialInterface*> ClothingMaterials;

};

/**
 * Defines a weapon item. Will use AO Weapon set in your EquipmentComponent and set it to the weapon you select.
 */
UCLASS()
class ALPHAOMEGA_API UAOEquipItem_Weapon : public UAOEquipItem
{
	GENERATED_BODY()

public:

	UAOEquipItem_Weapon();

	virtual void HandleEquip_Implementation() override;
	virtual void HandleUnequip_Implementation() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AAOWeapon> WeaponClass;
};