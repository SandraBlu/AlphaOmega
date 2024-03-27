// Fill out your copyright notice in the Description page of Project Settings.


#include "Attributes/AOAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Interface/CombatInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"


UAOAttributeSet::UAOAttributeSet()
{
	//const FAOGameplayTags& GameplayTags = FAOGameplayTags::Get();

// 	Primary Attributes
// 		AttributeTagMap.Add(GameplayTags.Attributes_Primary_Agility, GetAgilityAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Primary_Perception, GetPerceptionAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);
// 	
// 		//Secondary Attributes
// 		AttributeTagMap.Add(GameplayTags.Attributes_Secondary_Armor, GetArmorAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, GetCritHitChanceAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, GetCritHitDamageAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, GetCritHitResistanceAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Secondary_DodgeChance, GetDodgeChanceAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Secondary_Stealth, GetStealthAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Secondary_HealthRegen, GetHealthRegenAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Secondary_StaminaRegen, GetStaminaRegenAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Secondary_EnergyRegen, GetEnergyRegenAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Secondary_MaxStamina, GetMaxStaminaAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Secondary_MaxEnergy, GetMaxEnergyAttribute);
// 	
// 		AttributeTagMap.Add(GameplayTags.Attributes_Resistance_Dark, GetDarkResistanceAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Resistance_Electric, GetElectricResistanceAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Resistance_Fire, GetFireResistanceAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Resistance_Ice, GetIceResistanceAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Resistance_Physical, GetPhysicalResistanceAttribute);
// 		AttributeTagMap.Add(GameplayTags.Attributes_Resistance_Poison, GetPoisonResistanceAttribute);
}

void UAOAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	//Clamp Base Attributes
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	}
	if (Attribute == GetEnergyAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxEnergy());
	}
}

void UAOAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
		UE_LOG(LogTemp, Warning, TEXT("Changed Health on %s, Health: %f"), *Props.TargetAvatarActor->GetName(), GetHealth());
	}
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
	}
	if (Data.EvaluatedData.Attribute == GetEnergyAttribute())
	{
		SetEnergy(FMath::Clamp(GetEnergy(), 0.f, GetMaxEnergy()));
	}
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float LocalIncomingDamage = GetDamage();
		SetDamage(0.f);
		if (LocalIncomingDamage > 0.f)
		{
			const float NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

			const bool bFatal = NewHealth <= 0.f;
			if (bFatal)
			{
 				ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor);
 				if (CombatInterface)
 				{
// 					CombatInterface->Die();
 				}
			}
			/*else
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FAOGameplayTags::Get().Effects_HitReact);
				Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
			}
			const bool bBlock = UBFLAbilitySystem::IsBlockedHit(Props.ContextHandle);
			const bool bCriticalHit = UBFLAbilitySystem::IsCriticalHit(Props.ContextHandle);
			ShowFloatingText(Props, LocalIncomingDamage, bBlock, bCriticalHit);*/
		}
	}
}

void UAOAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	Props.ContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.ContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourcePC = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourcePC == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
			}
		}
		if (Props.SourcePC)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourcePC->GetPawn());
		}
	}
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);

	}
}
