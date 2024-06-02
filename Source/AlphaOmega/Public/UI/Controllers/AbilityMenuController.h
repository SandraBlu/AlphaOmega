// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Controllers/AOWidgetController.h"
#include "GameplayTagContainer.h"
#include "AOGameplayTags.h"
#include "AbilityMenuController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FAbilitySelectedSignature, bool, bSpendPointsBtnEnabled, bool, bActivateBtnEnabled, FString, DescriptionString, FString, NextLevelDescriptionString);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitActivateSelectionSignature, const FGameplayTag&, AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityReassignedSignature, const FGameplayTag&, AbilityType);

struct FSelectedAbility
{
	FGameplayTag Ability = FGameplayTag();
	FGameplayTag Status = FGameplayTag();
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class ALPHAOMEGA_API UAbilityMenuController : public UAOWidgetController
{
	GENERATED_BODY()

public:

	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;
	
	UPROPERTY(BlueprintAssignable)
	FOnPlayerStatChangeSignature AbilityPointsChanged;

	UPROPERTY(BlueprintAssignable)
	FAbilitySelectedSignature AbilitySelectedDelegate;

	UPROPERTY(BlueprintAssignable)
	FWaitActivateSelectionSignature WaitActivateDelegate;

	UPROPERTY(BlueprintAssignable)
	FWaitActivateSelectionSignature StopWaitActivateDelegate;

	UPROPERTY(BlueprintAssignable)
	FAbilityReassignedSignature AbilityReassigned;

	UFUNCTION(BlueprintCallable)
	void AbilitySelected(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable)
	void SpendPoint();

	UFUNCTION(BlueprintCallable)
	void AbilityDeselect();

	UFUNCTION(BlueprintCallable)
	void ActivateButtonPressed();

	UFUNCTION(BlueprintCallable)
	void AbilityInputButtonPressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType);

	void OnAbilityActivated(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PrevSlot);

private:
	
	static void ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 AbilityPoints, bool& bShouldEnableSpendPointsButton, bool& bShouldEnableActivateButton);
	FSelectedAbility SelectedAbility = { FAOGameplayTags::Get().ability_none,  FAOGameplayTags::Get().ability_status_locked };
	int32 CurrentAbilityPoints = 0;
	bool bWaitForActivateSelection = false;
	FGameplayTag SelectedInputSlot;
};
