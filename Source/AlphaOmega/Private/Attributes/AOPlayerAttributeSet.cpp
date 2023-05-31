// Fill out your copyright notice in the Description page of Project Settings.


#include "Attributes/AOPlayerAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Components/AOCoreComponent.h"

void UAOPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// This is called whenever attributes change, so for max health/stamina we want to scale the current totals to match
	Super::PreAttributeChange(Attribute, NewValue);

	// Handle Max Attributes
	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
		return;
	}

	if (Attribute == GetMaxStaminaAttribute())
	{
		AdjustAttributeForMaxChange(Stamina, MaxStamina, NewValue, GetStaminaAttribute());
		return;
	}

	if (Attribute == GetMaxEnergyAttribute())
	{
		AdjustAttributeForMaxChange(Energy, MaxEnergy, NewValue, GetEnergyAttribute());
		return;
	}
}

void UAOPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FAttributeExecutionData ExecutionData;
	GetExecutionDataFromMod(Data, ExecutionData);

	AActor* Instigator = ExecutionData.Instigator;
	AActor* TargetActor = ExecutionData.TargetActor;

	// And cast SourceActor / TargetActor to whatever Character classes you may be using and need access to

	const FGameplayTagContainer SourceTags = ExecutionData.SourceTags;
	const FGameplayEffectContextHandle Context = ExecutionData.Context;

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		HandleDamageAttribute(ExecutionData);
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaDamageAttribute())
	{
		HandleStaminaDamageAttribute(ExecutionData);
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		HandleHealthAttribute(ExecutionData);
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		HandleStaminaAttribute(ExecutionData);
	}
	else if (Data.EvaluatedData.Attribute == GetEnergyAttribute())
	{
		HandleEnergyAttribute(ExecutionData);
	}
}

void UAOPlayerAttributeSet::SetAttributeClamped(const FGameplayAttribute& Attribute, const float Value, const float MaxValue)
{
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	const float Min = GetClampMinimumValueFor(Attribute);
	const float NewValue = FMath::Clamp(Value, Min, MaxValue);

	ASC->SetNumericAttributeBase(Attribute, NewValue);
}

void UAOPlayerAttributeSet::HandleDamageAttribute(const FAttributeExecutionData& ExecutionData)
{
	AActor* Instigator = ExecutionData.Instigator;
	AActor* TargetActor = ExecutionData.TargetActor;
	UAOCoreComponent* TargetCoreComponent = ExecutionData.TargetCoreComponent;
	const FGameplayTagContainer SourceTags = ExecutionData.SourceTags;

	// Store a local copy of the amount of Damage done and clear the Damage attribute.
	const float LocalDamageDone = GetDamage();
	SetDamage(0.f);

	if (LocalDamageDone > 0.f)
	{
		// If actor was alive before damage is added, handle damage
		// This prevents damage being added to dead things and replaying death animations
		bool bAlive = true;

		if (TargetCoreComponent)
		{
			bAlive = TargetCoreComponent->IsAlive();
		}

		if (bAlive)
		{
			// Apply the Health change and then clamp it.
			const float NewHealth = GetHealth() - LocalDamageDone;
			const float ClampMinimumValue = GetClampMinimumValueFor(GetHealthAttribute());
			SetHealth(FMath::Clamp(NewHealth, ClampMinimumValue, GetMaxHealth()));

			if (TargetCoreComponent)
			{
				TargetCoreComponent->HandleDamage(LocalDamageDone, SourceTags, Instigator);
				TargetCoreComponent->HandleHealthChange(-LocalDamageDone, SourceTags);
			}
		}
	}
}

void UAOPlayerAttributeSet::HandleStaminaDamageAttribute(const FAttributeExecutionData& ExecutionData)
{
	UAOCoreComponent* TargetCoreComponent = ExecutionData.TargetCoreComponent;
	const FGameplayTagContainer SourceTags = ExecutionData.SourceTags;

	// Store a local copy of the amount of damage done and clear the damage attribute
	const float LocalStaminaDamageDone = GetStaminaDamage();
	SetStaminaDamage(0.f);

	if (LocalStaminaDamageDone > 0.0f)
	{
		// Apply the stamina change and then clamp it
		const float NewStamina = GetStamina() - LocalStaminaDamageDone;
		SetStamina(FMath::Clamp(NewStamina, 0.0f, GetMaxStamina()));

		if (TargetCoreComponent)
		{
			TargetCoreComponent->HandleStaminaChange(-LocalStaminaDamageDone, SourceTags);
		}
	}
}

void UAOPlayerAttributeSet::HandleHealthAttribute(const FAttributeExecutionData& ExecutionData)
{
	UAOCoreComponent* TargetCoreComponent = ExecutionData.TargetCoreComponent;
	const float ClampMinimumValue = GetClampMinimumValueFor(GetHealthAttribute());

	SetHealth(FMath::Clamp(GetHealth(), ClampMinimumValue, GetMaxHealth()));

	if (TargetCoreComponent)
	{
		const float DeltaValue = ExecutionData.DeltaValue;
		const FGameplayTagContainer SourceTags = ExecutionData.SourceTags;
		TargetCoreComponent->HandleHealthChange(DeltaValue, SourceTags);
	}
}

void UAOPlayerAttributeSet::HandleStaminaAttribute(const FAttributeExecutionData& ExecutionData)
{
	UAOCoreComponent* TargetCoreComponent = ExecutionData.TargetCoreComponent;
	const float ClampMinimumValue = GetClampMinimumValueFor(GetStaminaAttribute());

	SetStamina(FMath::Clamp(GetStamina(), ClampMinimumValue, GetMaxStamina()));

	if (TargetCoreComponent)
	{
		const float DeltaValue = ExecutionData.DeltaValue;
		const FGameplayTagContainer SourceTags = ExecutionData.SourceTags;
		TargetCoreComponent->HandleStaminaChange(DeltaValue, SourceTags);
	}
}

void UAOPlayerAttributeSet::HandleEnergyAttribute(const FAttributeExecutionData& ExecutionData)
{
	UAOCoreComponent* TargetCoreComponent = ExecutionData.TargetCoreComponent;
	const float ClampMinimumValue = GetClampMinimumValueFor(GetEnergyAttribute());

	SetEnergy(FMath::Clamp(GetEnergy(), ClampMinimumValue, GetMaxEnergy()));

	if (TargetCoreComponent)
	{
		const float DeltaValue = ExecutionData.DeltaValue;
		const FGameplayTagContainer SourceTags = ExecutionData.SourceTags;
		TargetCoreComponent->HandleEnergyChange(DeltaValue, SourceTags);
	}
}
