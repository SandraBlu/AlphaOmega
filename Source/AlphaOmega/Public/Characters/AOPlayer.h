// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/AOCharacter.h"
#include "UI/AOItemSlot.h"
#include "AOPlayer.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UAnimMontage;
class UAOInventoryComponent;
class UAOEquipItem;

USTRUCT()
struct FInteractData
{
	GENERATED_BODY()

	FInteractData()
	{
		ViewedInteractComp = nullptr;
		LastInteractCheckTime = 0.f;
		bInteractHeld = false;
	}

	UPROPERTY()
	class UAOInteractComponent* ViewedInteractComp;

	UPROPERTY()
	float LastInteractCheckTime;

	UPROPERTY()
	bool bInteractHeld;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquippedItemsChanged, const EEquipSlot, Slot, const UAOEquipItem*, Item);
/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API AAOPlayer : public AAOCharacter
{
	GENERATED_BODY()
	
public:

	AAOPlayer();

	//Add default meshes in case we do not have clothing item
	UPROPERTY(BlueprintReadOnly, Category = Mesh)
	TMap<EEquipSlot, USkeletalMesh*>  NakedMeshes;
	
	// Equip Map links slots to PlayerMeshes
	UPROPERTY(BlueprintReadOnly, Category = Mesh)
	TMap<EEquipSlot, USkeletalMeshComponent*> PlayerMeshes;
	
	//Player Meshes
	UPROPERTY(EditAnywhere, Category = "Components")
	class USkeletalMeshComponent* HeadAccessoryMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* TorsoMesh;
	UPROPERTY(EditAnywhere, Category = "Components")
	class USkeletalMeshComponent* LegsMesh;
	UPROPERTY(EditAnywhere, Category = "Components")
	class USkeletalMeshComponent* HandsMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* FeetMesh;
	UPROPERTY(EditAnywhere, Category = "Components")
	class USkeletalMeshComponent* BackpackMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DrawAnim;
	
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UAOInteractComponent* PlayerLootInteract;*/

	UFUNCTION(BlueprintCallable)
 	void PlayEquipMontage(const FName SectionName);

	UPROPERTY(EditDefaultsOnly, Category = "WeaponSocket")
	FName ActiveWeaponSocket;

protected:

	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	//virtual void Restart() override;

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* TPCCamBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* FollowTPC;

	
public:

	/*UPROPERTY(EditAnywhere, Category = "Weapon")
	class AAOWeapon* EquippedWeapon;*/


	//Interacting---------------------------------------
	//Holds Interact Info
	UPROPERTY()
	FInteractData InteractData;

	//Helper Function
	FORCEINLINE class UAOInteractComponent* GetInteractable() const { return InteractData.ViewedInteractComp; }

	UPROPERTY(EditDefaultsOnly, Category = "Interact")
	float InteractCheckFrequency;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceDistance;
	void PerformInteractCheck();
	void NoInteractableFound();
	void NewInteractableFound(UAOInteractComponent* Interactable);
	void Interact();

	FTimerHandle TimerHandle_Interact;

	//Timed Interact helpers----------
	bool IsInteracting() const;
	float GetRemainingInteractTime() const;

 	//-------------------------------------------
 	//Equipment Map holding current Equipped Items
 	UPROPERTY(VisibleAnywhere, Category = "Items")
 	TMap<EEquipSlot, UAOEquipItem*> EquippedItems;

	/// Combat============================>

	//UFUNCTION()
	//void OnWeaponEquipped();

	//bool CanAttack();

	//UFUNCTION(BlueprintCallable, Category = "Combat")
	//bool CanDrawWeapon();

	//UFUNCTION(BlueprintCallable, Category = "Combat")
	//void DrawWeapon();

	////Set Attack States/Called In Attack Anim Montage
	//UFUNCTION(BlueprintCallable, Category = "Combat")
	//void Attack();

	//UFUNCTION(BlueprintCallable)
	//void AttackEnd();

	//UFUNCTION(BlueprintCallable)
	//void Disarm();

	//UFUNCTION(BlueprintCallable)
	//void FinishDraw();

	//UFUNCTION(BlueprintCallable)
	//void SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled);


	//called when falling off cliff
	//void UntimelyDeath(struct FDamageEvent const& DamageEvent, const AActor* DamageCauser);

	/*UFUNCTION()
	void PlayTossFX(class UAnimMontage* TossMontage);
	class UThrowableItem* GetThrowable() const;
	void SpawnThrowable();
	bool CanUseThrowable() const;*/

	//Items-------------------
	UFUNCTION(BlueprintCallable, Category = "Items")
	void UseItem(class UAOItem* Item);

	UFUNCTION(BlueprintCallable, Category = "Items")
	void DropItem(class UAOItem* Item, const int32 Quantity);

 	UPROPERTY(EditDefaultsOnly, Category = "Item")
 	TSubclassOf<class AAOPickup> PickupClass;

 	//Equipment
 	bool EquipItem(class UAOEquipItem* Item);
 	bool UnequipItem(class UAOEquipItem* Item);
 
 //	void EquipGear(class UAOGearItem* Gear);
 //	void UnequipGear(const EEquipSlot Slot);
 
	//void EquipWeapon(class UAOWeaponItem* WeaponItem);
	//void UnequipWeapon();

 	//Equip Delegate
 	UPROPERTY(BlueprintAssignable, Category = "Item")
 	FOnEquippedItemsChanged OnEquippedItemsChanged;

 	//Equipment; BP Access SK Mesh Comp Slot
 	UFUNCTION(BlueprintPure)
 	class USkeletalMeshComponent* GetSlotSkeletalMeshComponent(const EEquipSlot Slot);
 
 	UFUNCTION(BlueprintPure)
 	FORCEINLINE TMap<EEquipSlot, UAOEquipItem*> GetEquippedItems() const { return EquippedItems; }

	/*UFUNCTION(BlueprintPure)
	FORCEINLINE class AAOWeapon* GetEquippedWeapon() const { return EquippedWeapon; }*/

	/*UFUNCTION(BlueprintPure)
	FORCEINLINE EEquipState GetEquipState() const { return EquipState; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE EActionState GetActionState() const { return ActionState; }*/

protected:

//Input 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* PlayerMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* InterAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ThrowAction;

private:

	/*EEquipState EquipState = EEquipState::EES_Unarmed;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Idle;*/
	
	//Input Binding Functions	
	void StartInteract(const FInputActionValue& Value);
	void CompleteInteract(const FInputActionValue& Value);

	//void UseThrowable(const FInputActionValue& Value);

public:	

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
