// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AOUserHUD.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "Attributes/AOPlayerAttributeSet.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UAOUserHUD::NativeConstruct()
{
	Super::NativeConstruct();

	if (APawn* OwningPlayerPawn = GetOwningPlayerPawn())
	{
		SetOwnerActor(OwningPlayerPawn);

		if (!TryInitAbilitySystem())
		{
			bLazyAbilitySystemInitialization = true;
		}
	}
}

void UAOUserHUD::NativeDestruct()
{
	// Clean up previously registered delegates for OwningPlayer AbilitySystemComponent
	ResetAbilitySystem();

	Super::NativeDestruct();
}

void UAOUserHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Lazy init disabled, was either good from native construct or initialized once here
	if (!bLazyAbilitySystemInitialization)
	{
		return;
	}

	if (TryInitAbilitySystem())
	{
		// We now have proper ASC and initialized widget, prevent further tick run
		bLazyAbilitySystemInitialization = false;
	}
}

void UAOUserHUD::InitFromCharacter()
{
	if (!AbilitySystemComponent)
	{
		// Prevent calls to GetAttributeValue if no ASC yet, which may happen for clients (and prevent warning logs during initialization)
		return;
	}

	SetHealth(GetAttributeValue(UAOPlayerAttributeSet::GetHealthAttribute()));
	SetStamina(GetAttributeValue(UAOPlayerAttributeSet::GetStaminaAttribute()));
	SetEnergy(GetAttributeValue(UAOPlayerAttributeSet::GetEnergyAttribute()));
}

void UAOUserHUD::SetMaxHealth(float MaxHealth)
{
	const float Health = GetAttributeValue(UAOPlayerAttributeSet::GetHealthAttribute());
	if (HealthText)
	{
		HealthText->SetText(FText::FromString(GetAttributeFormatString(Health, MaxHealth)));
	}

	if (MaxHealth != 0)
	{
		SetHealthPercentage(Health / MaxHealth);
	}
}

void UAOUserHUD::SetHealth(float Health)
{
	const float MaxHealth = GetAttributeValue(UAOPlayerAttributeSet::GetMaxHealthAttribute());
	if (HealthText)
	{
		HealthText->SetText(FText::FromString(GetAttributeFormatString(Health, MaxHealth)));
	}

	if (MaxHealth != 0)
	{
		SetHealthPercentage(Health / MaxHealth);
	}
}

void UAOUserHUD::SetHealthPercentage(float HealthPercentage)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(HealthPercentage);
	}
}

void UAOUserHUD::SetMaxStamina(float MaxStamina)
{
	const float Stamina = GetAttributeValue(UAOPlayerAttributeSet::GetStaminaAttribute());
	if (StaminaText)
	{
		StaminaText->SetText(FText::FromString(GetAttributeFormatString(Stamina, MaxStamina)));
	}

	if (MaxStamina != 0)
	{
		SetStaminaPercentage(Stamina / MaxStamina);
	}
}

void UAOUserHUD::SetStamina(float Stamina)
{
	const float MaxStamina = GetAttributeValue(UAOPlayerAttributeSet::GetMaxStaminaAttribute());
	if (StaminaText)
	{
		StaminaText->SetText(FText::FromString(GetAttributeFormatString(Stamina, MaxStamina)));
	}

	if (MaxStamina != 0)
	{
		SetStaminaPercentage(Stamina / MaxStamina);
	}
}

void UAOUserHUD::SetStaminaPercentage(float StaminaPercentage)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(StaminaPercentage);
	}
}

void UAOUserHUD::SetMaxEnergy(float MaxEnergy)
{
	const float Energy = GetAttributeValue(UAOPlayerAttributeSet::GetEnergyAttribute());
	if (EnergyText)
	{
		EnergyText->SetText(FText::FromString(GetAttributeFormatString(Energy, MaxEnergy)));
	}

	if (MaxEnergy != 0)
	{
		SetStaminaPercentage(Energy / MaxEnergy);
	}
}

void UAOUserHUD::SetEnergy(float Energy)
{
	const float MaxEnergy = GetAttributeValue(UAOPlayerAttributeSet::GetMaxEnergyAttribute());
	if (EnergyText)
	{
		EnergyText->SetText(FText::FromString(GetAttributeFormatString(Energy, MaxEnergy)));
	}

	if (MaxEnergy != 0)
	{
		SetStaminaPercentage(Energy / MaxEnergy);
	}
}

void UAOUserHUD::SetEnergyPercentage(float EnergyPercentage)
{
	if (EnergyProgressBar)
	{
		EnergyProgressBar->SetPercent(EnergyPercentage);
	}
}

void UAOUserHUD::HandleAttributeChange(FGameplayAttribute Attribute, float NewValue, float OldValue)
{
	if (Attribute == UAOPlayerAttributeSet::GetHealthAttribute())
	{
		SetHealth(NewValue);
	}
	else if (Attribute == UAOPlayerAttributeSet::GetStaminaAttribute())
	{
		SetStamina(NewValue);
	}
	else if (Attribute == UAOPlayerAttributeSet::GetEnergyAttribute())
	{
		SetEnergy(NewValue);
	}
	else if (Attribute == UAOPlayerAttributeSet::GetMaxHealthAttribute())
	{
		SetMaxHealth(NewValue);
	}
	else if (Attribute == UAOPlayerAttributeSet::GetMaxStaminaAttribute())
	{
		SetMaxStamina(NewValue);
	}
	else if (Attribute == UAOPlayerAttributeSet::GetMaxEnergyAttribute())
	{
		SetMaxEnergy(NewValue);
	}
}

FString UAOUserHUD::GetAttributeFormatString(float BaseValue, float MaxValue)
{
	return FString::Printf(TEXT("%d / %d"), FMath::FloorToInt(BaseValue), FMath::FloorToInt(MaxValue));
}

bool UAOUserHUD::TryInitAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor);
	}

	if (AbilitySystemComponent)
	{
		bLazyAbilitySystemInitialization = false;

		InitializeWithAbilitySystem(AbilitySystemComponent);

		// Init Stats
		InitFromCharacter();
	}

	return AbilitySystemComponent != nullptr;
}