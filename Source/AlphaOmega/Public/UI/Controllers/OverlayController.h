// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/Controllers/AOWidgetController.h"
#include "OverlayController.generated.h"


USTRUCT(BlueprintType)
struct FUITableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UAOUserWidget> MessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image = nullptr;

};
struct FOnAttributeChangeData;
class UAbilityInfo;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChange, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLevelChangedSignature, int32, NewLevel, bool, bLevelUp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUIMessageRowSignature, FUITableRow, Row);



/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class ALPHAOMEGA_API UOverlayController : public UAOWidgetController
{
	GENERATED_BODY()
	
public:

	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attribute")
	FOnAttributeChange OnHealthChange;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attribute")
	FOnAttributeChange OnMaxHealthChange;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attribute")
	FOnAttributeChange OnStaminaChange;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attribute")
	FOnAttributeChange OnMaxStaminaChange;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attribute")
	FOnAttributeChange OnEnergyChange;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attribute")
	FOnAttributeChange OnMaxEnergyChange;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FUIMessageRowSignature UIMessageRowDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|XP")
	FOnAttributeChange OnXPPercentChangeDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|XP")
	FOnLevelChangedSignature OnPlayerLevelChangeDelegate;

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	UDataTable* DTWidgetMessages;

	void OnXPChange(int32 NewXP);

	void OnAbilityActivated(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PrevSlot) const;

	//Handles Effect Messages on HUD
	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
};

template<typename T>
T* UOverlayController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}
