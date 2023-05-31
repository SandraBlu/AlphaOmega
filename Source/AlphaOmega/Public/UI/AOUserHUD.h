// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/AOUserWidget.h"
#include "AOUserHUD.generated.h"


struct FGameplayAbilitySpecHandle;
class UProgressBar;
class UTextBlock;

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UAOUserHUD : public UAOUserWidget
{
	GENERATED_BODY()

protected:
	//~ Begin UUserWidget interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~ End UUserWidget interface

public:

	/** Init widget with attributes from owner character **/
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void InitFromCharacter();

	/** Updates bound health widgets to reflect the new max health change */
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void SetMaxHealth(float MaxHealth);

	/** Updates bound health widgets to reflect the new health change */
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void SetHealth(float Health);

	/** Updates bound health progress bar with the new percent */
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void SetHealthPercentage(float HealthPercentage);

	/** Updates bound stamina widgets to reflect the new max stamina change */
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void SetMaxStamina(float MaxStamina);

	/** Updates bound stamina widgets to reflect the new stamina change */
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void SetStamina(float Stamina);

	/** Updates bound stamina progress bar with the new percent */
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void SetStaminaPercentage(float StaminaPercentage);

	/** Updates bound energy widgets to reflect the new max energy change */
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void SetMaxEnergy(float MaxEnergy);

	/** Updates bound energy widgets to reflect the new energy change */
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void SetEnergy(float Energy);

	/** Updates bound energy progress bar with the new percent */
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void SetEnergyPercentage(float EnergyPercentage);


protected:
	/** Set in native construct if called too early to check for ASC in tick, and kick off initialization logic when it is ready */
	bool bLazyAbilitySystemInitialization = false;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "UI")
		UTextBlock* HealthText = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "UI")
		UProgressBar* HealthProgressBar = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "UI")
		UTextBlock* StaminaText = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "UI")
		UProgressBar* StaminaProgressBar = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "UI")
		UTextBlock* EnergyText = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "UI")
		UProgressBar* EnergyProgressBar = nullptr;

	/** Updates bound widget whenever one of the attribute we care about is changed */
	virtual void HandleAttributeChange(FGameplayAttribute Attribute, float NewValue, float OldValue) override;


private:
	/** Array of active GE handle bound to delegates that will be fired when the count for the key tag changes to or away from zero */
	TArray<FActiveGameplayEffectHandle> GameplayEffectAddedHandles;

	/** Array of tags bound to delegates that will be fired when the count for the key tag changes to or away from zero */
	TArray<FGameplayTag> GameplayTagBoundToDelegates;

	static FString GetAttributeFormatString(float BaseValue, float MaxValue);

	/**
	 * Checks owner for a valid ASC and kick in initialization logic if it finds one
	 *
	 * @return Whether an ASC was found
	 */
	bool TryInitAbilitySystem();
	
};
