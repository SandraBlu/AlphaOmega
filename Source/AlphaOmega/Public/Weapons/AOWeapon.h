// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "GameplayTagContainer.h"
#include "AOWeapon.generated.h"

class AAOPlayer;
class USoundBase;
class UBoxComponent;
class UAnimMontage;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Unarmed UMETA(DisplayName = "Unarmed"),
	Melee UMETA(DisplayName = "Melee"),
	Ranged UMETA(DisplayName = "Ranged"),
	Skill UMETA(DisplayName = "Skill")
};

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Attacking UMETA(DisplayName = "Attacking"),  
	Equipping UMETA(DisplayName = "Equipping"),
};

/**
 *
 */

UCLASS()
class ALPHAOMEGA_API AAOWeapon : public AActor
{
	GENERATED_BODY()
	
public:	

	AAOWeapon();

	TArray<AActor*> IgnoreActors;

	UPROPERTY(EditDefaultsOnly, Category = "WeaponSocket")
	FName ActiveWeaponSocket;

	UPROPERTY(EditDefaultsOnly, Category = "WeaponSocket")
	FName InactiveWeaponSocket;

	/**collision mesh*/
	UPROPERTY(BlueprintReadWrite, Category = Components)
	USkeletalMeshComponent* SKMesh;

	//The weapon item in the players inventory
	UPROPERTY(BlueprintReadOnly)
	class UAOWeaponItem* Item;

	/**
	 * Animation Montages
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipAnim;

	/** weapon is being equipped by owner pawn */
	virtual void OnEquip();

	/** weapon is now equipped by owner pawn */
	virtual void OnEquipFinished();

	/** weapon is holstered by owner pawn */
	virtual void OnUnEquip();

	UFUNCTION(BlueprintCallable)
	bool IsAttacking() const;

	UFUNCTION(BlueprintCallable)
	bool IsEquipped() const;

	/** check if mesh is already attached */
	bool IsAttachedToPawn() const;

	UFUNCTION(BlueprintCallable)
	bool CanAttack() const;

/** attaches weapon mesh to pawn's mesh */
	void AttachMeshToPawn(USceneComponent* InParent, FName InSocketName);

	UFUNCTION(BlueprintCallable)
 	void PlayEquipMontage(FName SectionName);

	bool CanDisarm();
	bool CanDrawWeapon();

 	UPROPERTY(BlueprintReadWrite, Category = "AttackEvent")
 	FGameplayTag AttackEventTag;

	/** current weapon type */
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	EWeaponType CurrentType;

protected:

 //Anim Notify Functions
 	UFUNCTION(BlueprintCallable)
 	void Draw();
 	UFUNCTION(BlueprintCallable)
 	void Disarm();
 	
 	void FinishDraw();

	UFUNCTION(BlueprintPure, Category = "Weapon")
	EWeaponState GetCurrentState() const;

	UFUNCTION(BlueprintPure, Category = "Weapon")
	EWeaponType GetCurrentType() const;
	
	/** is weapon currently equipped? */
	uint32 bIsEquipped : 1;

	/** is equip animation playing? */
	uint32 bPendingEquip : 1;

	/** is attack animation playing? */
	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	bool bIsAttacking;

	/** is weapon type melee? */
	uint32 bIsMeleeType : 1;

	/** current weapon state */
	EWeaponState CurrentState;

	

	/** update weapon state */
	void SetWeaponState(EWeaponState NewState);

	/** update weapon state */
	void SetWeaponType(EWeaponType NewType);

	/** determine current weapon state */
	void DetermineWeaponState();

	/** determine current weapon state */
	void DetermineWeaponType();

	virtual void BeginPlay() override;
		
	UFUNCTION(BlueprintImplementableEvent)
	void CreateForceFields(const FVector& FieldLocation);

	/** get weapon mesh (needs pawn owner to determine variant) */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	USkeletalMeshComponent* GetWeaponMesh() const;

	/** get pawn owner */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	class AAOPlayer* GetPawnOwner() const;

	UPROPERTY()
	class AAOPlayer* PawnOwner;

	/** single fire sound (bLoopedFireSound not set) */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundBase* AttackSound;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	USoundBase* DrawSound;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	UBoxComponent* WeaponBox;

	

};
