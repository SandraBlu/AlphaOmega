// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AOCharacter.h"
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
	class UInteractComponent* ViewedInteractComp;

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
	UPROPERTY(EditAnywhere, Category = "Components")
	class USkeletalMeshComponent* Canteen;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UInteractComponent* PlayerLootInteract;

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	//virtual void Restart() override;

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* FollowCam;

	
public:

	UPROPERTY(EditAnywhere, Category = "Weapon")
	class AAOWeapon* EquippedWeapon;


	//Interacting---------------------------------------
	//Holds Interact Info
	UPROPERTY()
	FInteractData InteractData;

	//Helper Function
	FORCEINLINE class UInteractComponent* GetInteractable() const { return InteractData.ViewedInteractComp; }

	UPROPERTY(EditDefaultsOnly, Category = "Interact")
	float InteractCheckFrequency;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceDistance;
	void PerformInteractCheck();
	void NoInteractableFound();
	void NewInteractableFound(UInteractComponent* Interactable);
	void Interact();

	FTimerHandle TimerHandle_Interact;

 	//-------------------------------------------
 	//Equipment Map holding current Equipped Items
 	UPROPERTY(VisibleAnywhere, Category = "Items")
 	TMap<EEquipSlot, UAOEquipItem*> EquippedItems;

	UFUNCTION()
	void OnWeaponEquipped();

	//called when falling off cliff
	//void UntimelyDeath(struct FDamageEvent const& DamageEvent, const AActor* DamageCauser);

	UFUNCTION()
	void PlayTossFX(class UAnimMontage* TossMontage);
	class UThrowableItem* GetThrowable() const;
	void SpawnThrowable();
	bool CanUseThrowable() const;

public:

	//Timed Interact helpers----------
	bool IsInteracting() const;
	float GetRemainingInteractTime() const;

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
 
 	void EquipGear(class UAOGearItem* Gear);
 	void UnequipGear(const EEquipSlot Slot);
 
 	void EquipWeapon(class UAOWeaponItem* WeaponItem);
 	void UnequipWeapon();

 	//Equip Delegate
 	UPROPERTY(BlueprintAssignable, Category = "Item")
 	FOnEquippedItemsChanged OnEquippedItemsChanged;

 protected:
	//Aiming-----------------
	bool CanAim() const;

	void SetAiming(const bool bNewAiming);

	UPROPERTY(BlueprintReadWrite)
	bool bIsAiming;

	UPROPERTY(BlueprintReadWrite)
	bool bIsSprinting = false;

	UPROPERTY(BlueprintReadWrite)
	float SprintSpeed;

	UPROPERTY(BlueprintReadWrite)
	float JogSpeed;



 public: 
 	//Equipment; BP Access SK Mesh Comp Slot
 	UFUNCTION(BlueprintPure)
 	class USkeletalMeshComponent* GetSlotSkeletalMeshComponent(const EEquipSlot Slot);
 
 	UFUNCTION(BlueprintPure)
 	FORCEINLINE TMap<EEquipSlot, UAOEquipItem*> GetEquippedItems() const { return EquippedItems; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE class AAOWeapon* GetEquippedWeapon() const { return EquippedWeapon; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsAiming() const { return bIsAiming; }


protected:

//Input 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* PlayerMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* InterAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* OpenMenuAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* OpenMapAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* DrawWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ThrowAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AimAction;
	
	//void StartReload();
	//Input Binding Functions
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void CrouchStart(const FInputActionValue& Value);
	void CrouchStop(const FInputActionValue& Value);
		
	void StartInteract(const FInputActionValue& Value);
	void CompleteInteract(const FInputActionValue& Value);

	void DrawWeapon(const FInputActionValue& Value);
	void UseThrowable(const FInputActionValue& Value);
	void StartAiming(const FInputActionValue& Value);
	void StopAiming(const FInputActionValue& Value);
	
	void OpenMenu(const FInputActionValue& Value);
	void OpenMap(const FInputActionValue& Value);

public:	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
