// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/AOEquipItem.h"
#include "AOEquipmentComponent.generated.h"

/**Called on server when an item is added to this inventory*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemEquipped, const EEquipSlot, Slot, class UAOEquipItem*, Equippable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemUnequipped, const EEquipSlot, Slot, class UAOEquipItem*, Equippable);

USTRUCT()
struct FDefaultClothingMeshMaterials
{

	GENERATED_BODY()

	FDefaultClothingMeshMaterials(){};
	FDefaultClothingMeshMaterials(TArray<class UMaterialInterface*> InMaterials) : Materials(InMaterials){};

	UPROPERTY()
	TArray<class UMaterialInterface*> Materials;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALPHAOMEGA_API UAOEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAOEquipmentComponent();

friend class UAOEquipItem;
	friend class AAOWeapon;

	/**Initialize the equipment component, by telling it which meshes link to which slot.
	
	@param ClothingMeshes The map which maps each clothing slot to the skeletal mesh component the clothing will equip to 
	@param LeaderPoseComponent the component all of the equipped items will be told to follow upon equipping. */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	virtual void Initialize(TMap<EEquipSlot, USkeletalMeshComponent*> ClothingMeshes, class USkeletalMeshComponent* LeaderPoseComponent);

	//Return the item equipped at the given slot
	UFUNCTION(BlueprintPure, Category = "Equipment")
	class UAOEquipItem* GetEquippedItemAtSlot(const EEquipSlot Slot);

	//Return how much all of our equipped items weigh
	UFUNCTION(BlueprintPure, Category = "Equipment")
	virtual float GetEquippedItemsWeight() const;

	UPROPERTY(BlueprintAssignable, Category = "Equipment")
	FOnItemEquipped OnItemEquipped;

	UPROPERTY(BlueprintAssignable, Category = "Equipment")
	FOnItemUnequipped OnItemUnequipped;

	//Mark the item as equipped/unequipped
	virtual void EquipItem(class UAOEquipItem* Equippable);
	virtual void UnequipItem(class UAOEquipItem* Equippable);

	/**When we put a new item on, we need to tell it to follow the leader pose component, so we store that here. */
	UPROPERTY()
	class USkeletalMeshComponent* LeaderPoseComponent;

	/**We cache this so when an item is removed we can set it back to the default*/
	UPROPERTY()
	TMap<EEquipSlot, USkeletalMesh*> DefaultClothing;

	/**We cache this so when an item is removed we can set it back to the default materials*/
	UPROPERTY()
	TMap<EEquipSlot, FDefaultClothingMeshMaterials> DefaultClothingMaterials;

	/**The skeletal meshes we'll be changing if a player equips an item*/
	UPROPERTY()
	TMap<EEquipSlot, USkeletalMeshComponent*> EquippableComponents;

	/**All of the items that are currently equipped are stored in here*/
	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	TMap<EEquipSlot, UAOEquipItem*> EquippedItems;
	
	UFUNCTION()
	void OnWeaponEquipped();
	
	UPROPERTY(EditAnywhere, Category = "Weapon")
	class AAOWeapon* EquippedWeapon;

	UFUNCTION(BlueprintPure)
	FORCEINLINE AAOWeapon* GetEquippedWeapon() const { return EquippedWeapon; }

public:

	FORCEINLINE class USkeletalMeshComponent* GetLeaderPoseComponent() const {return LeaderPoseComponent;};
	FORCEINLINE TMap<EEquipSlot, USkeletalMeshComponent*> GetEquippableComponents() const {return EquippableComponents;};
	FORCEINLINE TMap<EEquipSlot, USkeletalMesh*> GetDefaultClothing() const {return DefaultClothing;};
		
};