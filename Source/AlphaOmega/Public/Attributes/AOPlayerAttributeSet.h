// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attributes/AOBaseAttributeSet.h"
#include "AOBaseAttributeSet.h"
#include "AOPlayerAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UAOPlayerAttributeSet : public UAOBaseAttributeSet
{
	GENERATED_BODY()
	
public:

	// AttributeSet Overrides
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// Current Health, when 0 we expect owner to die unless prevented by an ability. Capped by MaxHealth.
	// Positive changes can directly use this.
	// Negative changes to Health should go through Damage meta attribute.
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Health = 0.0f;
	ATTRIBUTE_ACCESSORS(UAOPlayerAttributeSet, Health)

	// MaxHealth is its own attribute since GameplayEffects may modify it
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxHealth = 0.0f;
	ATTRIBUTE_ACCESSORS(UAOPlayerAttributeSet, MaxHealth)

	// Health regen rate will passively increase Health every period
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData HealthRegenRate = 0.0f;
	ATTRIBUTE_ACCESSORS(UAOPlayerAttributeSet, HealthRegenRate)

	// Current stamina, used to execute abilities. Capped by MaxStamina.
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Stamina = 0.0f;
	ATTRIBUTE_ACCESSORS(UAOPlayerAttributeSet, Stamina)

	// MaxStamina is its own attribute since GameplayEffects may modify it
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxStamina = 0.0f;
	ATTRIBUTE_ACCESSORS(UAOPlayerAttributeSet, MaxStamina)

	// Stamina regen rate will passively increase Stamina every period
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData StaminaRegenRate = 0.0f;
	ATTRIBUTE_ACCESSORS(UAOPlayerAttributeSet, StaminaRegenRate)

	// Current Energy, used to execute special abilities. Capped by MaxEnergy.
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Energy = 0.0f;
	ATTRIBUTE_ACCESSORS(UAOPlayerAttributeSet, Energy)

	// MaxEnergy is its own attribute since GameplayEffects may modify it
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxEnergy = 0.0f;
	ATTRIBUTE_ACCESSORS(UAOPlayerAttributeSet, MaxEnergy)

	//Strength will increase attack 
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData AttackPower = 0.0f;
	ATTRIBUTE_ACCESSORS(UAOPlayerAttributeSet, AttackPower)

	//Stealth will decrease AI sight
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Stealth = 0.0f;
	ATTRIBUTE_ACCESSORS(UAOPlayerAttributeSet, Stealth)

	//Resistance will decrease damage - various types handled by gameplay tags
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData DamageResistance = 0.0f;
	ATTRIBUTE_ACCESSORS(UAOPlayerAttributeSet, DamageResistance)

	//MoveSpeed will increase Attack
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MoveSpeed = 0.0f;
	ATTRIBUTE_ACCESSORS(UAOPlayerAttributeSet, MoveSpeed)

	// Damage is a meta attribute used by the DamageExecution to calculate final damage, which then turns into -Health
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", meta = (HideFromLevelInfos))
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UAOPlayerAttributeSet, Damage)

	// StaminaDamage is a meta attribute used by the DamageExecution to calculate final damage, which then turns into -Stamina
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", meta = (HideFromLevelInfos))
	FGameplayAttributeData StaminaDamage;
	ATTRIBUTE_ACCESSORS(UAOPlayerAttributeSet, StaminaDamage)

	virtual void SetAttributeClamped(const FGameplayAttribute& Attribute, const float Value, const float MaxValue);

	virtual void HandleDamageAttribute(const FAttributeExecutionData& ExecutionData);
	virtual void HandleStaminaDamageAttribute(const FAttributeExecutionData& ExecutionData);
	virtual void HandleHealthAttribute(const FAttributeExecutionData& ExecutionData);
	virtual void HandleStaminaAttribute(const FAttributeExecutionData& ExecutionData);
	virtual void HandleEnergyAttribute(const FAttributeExecutionData& ExecutionData);
};
