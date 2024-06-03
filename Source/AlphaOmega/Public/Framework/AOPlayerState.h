// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AOPlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UXPInfo;
class UAOInventoryComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32 /*StatValue*/)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLevelChanged, int32 /*StatValue*/, bool /*bLevelUp*/)

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API AAOPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:

	AAOPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() { return AttributeSet; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class UAOInventoryComponent* PlayerInventory;

	UPROPERTY(EditDefaultsOnly)
	UXPInfo* XPInfo;

	FOnPlayerStatChanged OnXPChangeDelegate;
	FOnLevelChanged OnLevelChangeDelegate;
	FOnPlayerStatChanged OnAttributePtsChangeDelegate;
	FOnPlayerStatChanged OnAbilityPtsChangeDelegate;

	FORCEINLINE int32 GetCharacterLevel() const { return Level; }
	FORCEINLINE int32 GetXP() const { return XP; }
	FORCEINLINE int32 GetAttributePts() const { return AttributePts; }
	FORCEINLINE int32 GetAbilityPts() const { return AbilityPts; }

	void AddToLevel(int32 InLevel);
	void AddToXP(int32 InXP);
	void AddToAttributePts(int32 InAttributePts);
	void AddToAbilityPts(int32 InAbilityPts);

	void SetLevel(int32 InLevel);
	void SetXP(int32 InXP);
	void SetAttributePoints(int32 InPoints);
	void SetAbilityPoints(int32 InPoints);


protected:

	UPROPERTY()
	UAttributeSet* AttributeSet;

	UPROPERTY(VisibleAnywhere)
	UAbilitySystemComponent* AbilitySystemComponent;

	
private:

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 Level = 1;
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_XP)
	int32 XP = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AttributePts)
	int32 AttributePts = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AbilityPts)
	int32 AbilityPts = 0;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UFUNCTION()
	void OnRep_XP(int32 OldXP);

	UFUNCTION()
	void OnRep_AttributePts(int32 OldAttributePts);

	UFUNCTION()
	void OnRep_AbilityPts(int32 OldAbilityPts);

};
