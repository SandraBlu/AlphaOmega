// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AOAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT(BlueprintType)
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayEffectContextHandle ContextHandle;
	//Source
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAbilitySystemComponent* SourceASC = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* SourceAvatarActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AController* SourcePC = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACharacter* SourceCharacter = nullptr;

	//Target
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAbilitySystemComponent* TargetASC = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* TargetAvatarActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AController* TargetController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACharacter* TargetCharacter = nullptr;

};

template<class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;
/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UAOAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	
	UAOAttributeSet();

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	//Attribute Map
	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> AttributeTagMap;

private:

	void HandleDamage(const FEffectProperties& Props);
	void HandleXP(const FEffectProperties& Props);
	void HandleDebuff(const FEffectProperties& Props);
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
	void ShowFloatingText(const FEffectProperties& Props, float DamageAmount, bool bBlockedHit, bool bCriticalHit) const;
	void SendXPEvent(const FEffectProperties& Props);
	bool bRefillHealth = false;
	bool bRefillStamina = false;
	bool bRefillEnergy = false;
	


public:

	//Base Attributes-----------------------
	UPROPERTY(BlueprintReadOnly, Category = "Base Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Base Attributes")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, Stamina)

	UPROPERTY(BlueprintReadOnly, Category = "Base Attributes")
	FGameplayAttributeData Energy;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, Energy)

	/*
	 * Meta Attributes
	 */

	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, Damage);

	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData XP;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, XP);
	

	//RPG Attributes-----------------------------

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Agility;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, Agility)

	//Stealth will decrease AI sight
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, Intelligence)

	//Resistance will decrease damage - various types handled by gameplay tags
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Perception;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, Perception)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, Resilience)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, Strength)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, Vigor)

	//Secondary Attributes------------------------
	UPROPERTY(BlueprintReadOnly, Category = "Base Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Base Attributes")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, MaxStamina)

	UPROPERTY(BlueprintReadOnly, Category = "Base Attributes")
	FGameplayAttributeData MaxEnergy;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, MaxEnergy)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, Armor)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, ArmorPenetration)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData BlockChance;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, BlockChance)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData DodgeChance;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, DodgeChance)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Stealth;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, Stealth)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData CritHitChance;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, CritHitChance)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData CritHitDamage;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, CritHitDamage)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData CritHitResistance;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, CritHitResistance)

	UPROPERTY(BlueprintReadOnly, Category = "Base Attributes")
	FGameplayAttributeData HealthRegen;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, HealthRegen)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData StaminaRegen;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, StaminaRegen)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData EnergyRegen;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, EnergyRegen)

	//Resistances
	UPROPERTY(BlueprintReadOnly, Category = "Resistance Attributes")
	FGameplayAttributeData PhysicalResistance;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, PhysicalResistance);

	UPROPERTY(BlueprintReadOnly, Category = "Resistance Attributes")
	FGameplayAttributeData PoisonResistance;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, PoisonResistance);

	UPROPERTY(BlueprintReadOnly, Category = "Resistance Attributes")
	FGameplayAttributeData DarkResistance;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, DarkResistance);

	UPROPERTY(BlueprintReadOnly, Category = "Resistance Attributes")
	FGameplayAttributeData ElectricResistance;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, ElectricResistance);

	UPROPERTY(BlueprintReadOnly, Category = "Resistance Attributes")
	FGameplayAttributeData FireResistance;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, FireResistance);

	UPROPERTY(BlueprintReadOnly, Category = "Resistance Attributes")
	FGameplayAttributeData IceResistance;
	ATTRIBUTE_ACCESSORS(UAOAttributeSet, IceResistance);

};
