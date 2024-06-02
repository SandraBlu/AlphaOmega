// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "GameplayTagContainer.h"
#include "AOWeapon.generated.h"

UENUM(BlueprintType)
enum class ECombatType : UINT8
{
	ECT_None UMETA(DisplayName = "None"),
	ECT_LightSword UMETA(DisplayName = "Sword"),
	ECT_Staff UMETA(DisplayName = "Staff"),
	ECT_Ranged UMETA(DisplayName = "Ranged")
};

class AAOPlayer;
class USoundBase;
class UBoxComponent;
class UAnimMontage;

UCLASS()
class ALPHAOMEGA_API AAOWeapon : public AActor
{
	GENERATED_BODY()

public:

	AAOWeapon();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings")
	ECombatType CombatType;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	FName ActiveWeaponSocket;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	FName InactiveWeaponSocket;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	FName FiringSocket;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	UAnimMontage* EquipAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	USoundBase* EquipSFX;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	USoundBase* DisarmSFX;

	//Actors to ignore after weapon hits them (same swing)
	TArray<AActor*> IgnoreActors;

	/**collision mesh*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components)
	USkeletalMeshComponent* SKMesh;

	UFUNCTION(BlueprintCallable)
	void PlayEquipMontage(const FName SectionName);

	/** weapon is being equipped by owner pawn */
	UFUNCTION(BlueprintCallable)
	virtual void OnEquip();

	/** weapon is holstered by owner pawn */
	UFUNCTION(BlueprintCallable)
	virtual void OnUnEquip();

	/** attaches weapon mesh to pawn's mesh */
	void AttachMeshToPawn(USceneComponent* InParent, FName InSocketName);

	/** get weapon mesh (needs pawn owner to determine variant) */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	USkeletalMeshComponent* GetWeaponMesh() const;

	UFUNCTION(BlueprintCallable)
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//Public getter for private weapon box
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }

	UFUNCTION(BlueprintImplementableEvent)
	void OnHit(FHitResult BoxHit);

	void UpdateCombatType(ECombatType);

protected:

	virtual void BeginPlay() override;

	//Implement Field System on Weapon
	UFUNCTION(BlueprintImplementableEvent)
	void CreateForceFields(const FVector& FieldLocation);

	/** get pawn owner */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	class AAOPlayer* GetPawnOwner() const;

	UPROPERTY()
	class AAOPlayer* PawnOwner;


private:
	//Weapon Box Settings
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	USceneComponent* TraceStart;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	USceneComponent* TraceEnd;

};