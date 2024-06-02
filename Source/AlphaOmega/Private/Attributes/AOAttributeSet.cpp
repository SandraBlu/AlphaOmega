// Fill out your copyright notice in the Description page of Project Settings.


#include "Attributes/AOAttributeSet.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffect.h"
#include "Framework/AOPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "AOGameplayTags.h"
#include "Interfaces/CombatInterface.h"
#include "Interfaces/PlayerInterface.h"
#include <Framework/AOBFL.h>
#include "AOGameplayEffectTypes.h"
#include "AlphaOmega/AOLogChannel.h"

UAOAttributeSet::UAOAttributeSet()
{
	const FAOGameplayTags& GameplayTags = FAOGameplayTags::Get();

	//Primary Attributes
	AttributeTagMap.Add(GameplayTags.Attributes_Primary_Agility, GetAgilityAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Primary_Perception, GetPerceptionAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);

	//Secondary Attributes
	AttributeTagMap.Add(GameplayTags.Attributes_Secondary_Armor, GetArmorAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, GetCritHitChanceAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, GetCritHitDamageAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, GetCritHitResistanceAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Secondary_DodgeChance, GetDodgeChanceAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Secondary_Stealth, GetStealthAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Secondary_HealthRegen, GetHealthRegenAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Secondary_StaminaRegen, GetStaminaRegenAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Secondary_EnergyRegen, GetEnergyRegenAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Secondary_MaxStamina, GetMaxStaminaAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Secondary_MaxEnergy, GetMaxEnergyAttribute);

	AttributeTagMap.Add(GameplayTags.Attributes_Resistance_Dark, GetDarkResistanceAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Resistance_Electric, GetElectricResistanceAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Resistance_Fire, GetFireResistanceAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Resistance_Ice, GetIceResistanceAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Resistance_Physical, GetPhysicalResistanceAttribute);
	AttributeTagMap.Add(GameplayTags.Attributes_Resistance_Poison, GetPoisonResistanceAttribute);
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

void UAOAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute() && bRefillHealth)
	{
		SetHealth(GetMaxHealth());
	}
	if (Attribute == GetMaxStaminaAttribute() && bRefillStamina)
	{
		SetStamina(GetMaxStamina());
	}
	if (Attribute == GetMaxEnergyAttribute() && bRefillEnergy)
	{
		SetEnergy(GetMaxEnergy());
	}
}

void UAOAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	if (Props.TargetCharacter->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(Props.TargetCharacter)) return;

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
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
		HandleDamage(Props);
	}
	if (Data.EvaluatedData.Attribute == GetXPAttribute())
	{
		HandleXP(Props);
	}
}

void UAOAttributeSet::HandleDamage(const FEffectProperties& Props)
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
				CombatInterface->Die();
			}
			SendXPEvent(Props);
		}
		else
		{
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(FAOGameplayTags::Get().ability_HitReact);
			Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
		}
		const bool bBlock = UAOBFL::IsBlockedHit(Props.ContextHandle);
		const bool bCritHit = UAOBFL::IsCriticalHit(Props.ContextHandle);
		ShowFloatingText(Props, LocalIncomingDamage, bBlock, bCritHit);
		if (UAOBFL::IsSuccessfulDebuff(Props.ContextHandle))
		{
			HandleDebuff(Props);
		}
	}
}

void UAOAttributeSet::HandleXP(const FEffectProperties& Props)
{
	const float LocalXP = GetXP();
	SetXP(0.f);

	if (Props.SourceCharacter->Implements<UPlayerInterface>() && Props.SourceCharacter->Implements<UCombatInterface>())
	{
		const int32 CurrentLevel = ICombatInterface::Execute_GetCharacterLevel(Props.SourceCharacter);
		const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Props.SourceCharacter);

		const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(Props.SourceCharacter, CurrentXP + LocalXP);
		const int32 TotalLevelUps = NewLevel - CurrentLevel;
		if (TotalLevelUps > 0)
		{
			//Get attr/ability Points reward
			const int32 AttributePtsReward = IPlayerInterface::Execute_GetAttributePointsReward(Props.SourceCharacter, CurrentLevel);
			const int32 AbilityPtsReward = IPlayerInterface::Execute_GetAbilityPointsReward(Props.SourceCharacter, CurrentLevel);
			//Add to player level, Add Attr Point and Ability Point Rewards, fill up base attributes
			IPlayerInterface::Execute_AddToPlayerLevel(Props.SourceCharacter, TotalLevelUps);
			IPlayerInterface::Execute_AddToAttributePoints(Props.SourceCharacter, AttributePtsReward);
			IPlayerInterface::Execute_AddToAbilityPoints(Props.SourceCharacter, AbilityPtsReward);

			bRefillHealth = true;
			bRefillStamina = true;
			bRefillEnergy = true;

			IPlayerInterface::Execute_LevelUp(Props.SourceCharacter);
		}

		IPlayerInterface::Execute_AddToXP(Props.SourceCharacter, LocalXP);
	}
}

void UAOAttributeSet::HandleDebuff(const FEffectProperties& Props)
{
	const FAOGameplayTags& GameplayTags = FAOGameplayTags::Get();
	FGameplayEffectContextHandle EffectContext = Props.SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(Props.SourceAvatarActor);

	const FGameplayTag DamageType = UAOBFL::GetDamageType(Props.ContextHandle);
	const float DebuffDamage = UAOBFL::GetDebuffDamage(Props.ContextHandle);
	const float DebuffDuration = UAOBFL::GetDebuffDuration(Props.ContextHandle);
	const float DebuffFrequency = UAOBFL::GetDebuffFrequency(Props.ContextHandle);

	FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DamageType.ToString());
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName));

	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	Effect->Period = DebuffFrequency;
	Effect->DurationMagnitude = FScalableFloat(DebuffDuration);

	Effect->InheritableOwnedTagsContainer.AddTag(GameplayTags.DamageTypeToDebuff[DamageType]);

	Effect->StackingType = EGameplayEffectStackingType::AggregateBySource;
	Effect->StackLimitCount = 1;

	const int32 Index = Effect->Modifiers.Num();
	Effect->Modifiers.Add(FGameplayModifierInfo());
	FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[Index];

	ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.Attribute = UAOAttributeSet::GetDamageAttribute();

	if (FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContext, 1.f))
	{
		FAOGameplayEffectContext* AOContext = static_cast<FAOGameplayEffectContext*>(MutableSpec->GetContext().Get());
		TSharedPtr<FGameplayTag> DebuffDamageType = MakeShareable(new FGameplayTag(DamageType));
		AOContext->SetDamageType(DebuffDamageType);

		Props.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);
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

void UAOAttributeSet::ShowFloatingText(const FEffectProperties& Props, float DamageAmount, bool bBlockedHit, bool bCriticalHit) const
{
if (Props.SourceCharacter != Props.TargetCharacter)
	{
		if (AAOPlayerController* PC = Cast<AAOPlayerController>(UGameplayStatics::GetPlayerController(Props.SourceCharacter, 0)))
		{
			PC->ShowDamageText(DamageAmount, Props.TargetCharacter, bBlockedHit, bCriticalHit);
			return;
		}
		if (AAOPlayerController* PC = Cast<AAOPlayerController>(UGameplayStatics::GetPlayerController(Props.TargetCharacter, 0)))
		{
			PC->ShowDamageText(DamageAmount, Props.TargetCharacter, bBlockedHit, bCriticalHit);
		}
	}
}

void UAOAttributeSet::SendXPEvent(const FEffectProperties& Props)
{
	if (Props.TargetCharacter->Implements<UCombatInterface>())
	{
		const int32 TargetLevel = ICombatInterface::Execute_GetCharacterLevel(Props.TargetCharacter);
		const ECharacterClass TargetClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);
		const int32 XPReward = UAOBFL::GetXPRewardForClassAndLevel(Props.TargetCharacter, TargetClass, TargetLevel);
		const FAOGameplayTags& GameplayTags = FAOGameplayTags::Get();

		FGameplayEventData Payload;
		Payload.EventTag = GameplayTags.Attributes_meta_xp;
		Payload.EventMagnitude = XPReward;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, GameplayTags.Attributes_meta_xp, Payload);
	}
}
