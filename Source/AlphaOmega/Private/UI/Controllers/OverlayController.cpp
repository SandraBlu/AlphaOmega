// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Controllers/OverlayController.h"
#include "Attributes/AOAttributeSet.h"
#include "AbilitySystem/AOAbilityComp.h"
#include <AbilitySystem/Data/AbilityInfo.h>
#include <Framework/AOPlayerState.h>
#include <AbilitySystem/Data/XPInfo.h>
#include "AOGameplayTags.h"

void UOverlayController::BroadcastInitialValues()
{
	OnHealthChange.Broadcast(GetAOAS()->GetHealth());
	OnMaxHealthChange.Broadcast(GetAOAS()->GetMaxHealth());
	OnStaminaChange.Broadcast(GetAOAS()->GetStamina());
	OnMaxStaminaChange.Broadcast(GetAOAS()->GetMaxStamina());
	OnEnergyChange.Broadcast(GetAOAS()->GetEnergy());
	OnMaxEnergyChange.Broadcast(GetAOAS()->GetMaxEnergy());
	
}

void UOverlayController::BindCallbacksToDependencies()
{
	//XP Change
	GetAOPS()->OnXPChangeDelegate.AddUObject(this, &UOverlayController::OnXPChange);
	GetAOPS()->OnLevelChangeDelegate.AddLambda(	[this](int32 NewLevel, bool bLevelUp)
		{
			OnPlayerLevelChangeDelegate.Broadcast(NewLevel, bLevelUp);
		});
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAOAS()->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) {OnHealthChange.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAOAS()->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) {OnMaxHealthChange.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAOAS()->GetStaminaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) {OnStaminaChange.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAOAS()->GetMaxStaminaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) {OnMaxStaminaChange.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAOAS()->GetEnergyAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) {OnEnergyChange.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAOAS()->GetMaxEnergyAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) {OnMaxEnergyChange.Broadcast(Data.NewValue); });
	
	if (GetAOASC())
	{
		GetAOASC()->AbilityActivated.AddUObject(this, &UOverlayController::OnAbilityActivated);
		if (GetAOASC()->bAbilityGranted)
		{
			BroadcastAbilityInfo();
		}
		else 
		{
			GetAOASC()-> AbilityGivenDelegate.AddUObject(this, &UOverlayController::BroadcastAbilityInfo);
		}
		
		GetAOASC()->EffectTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("message"));
				if (Tag.MatchesTag(MessageTag))
				{
					const FUITableRow* Row = GetDataTableRowByTag<FUITableRow>(DTWidgetMessages, Tag);
					UIMessageRowDelegate.Broadcast(*Row);
				}
			}
		}
	);
	}
}

void UOverlayController::OnXPChange(int32 NewXP)
{
	const UXPInfo* XPInfo = GetAOPS()->XPInfo;
	checkf(XPInfo, TEXT("Unable to find XPinfo, Enter in BP_PlayerState"));

	const int32 CurrentLevel = XPInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = XPInfo->LevelUpInfo.Num();

	if (CurrentLevel <= MaxLevel && CurrentLevel > 0)
	{
		const int32 LevelUpReq = XPInfo->LevelUpInfo[CurrentLevel].LevelUpPointsRequired;
		const int32 PrevLevelUpReq = XPInfo->LevelUpInfo[CurrentLevel -1].LevelUpPointsRequired;

		const int32 DeltaLevelReq = LevelUpReq - PrevLevelUpReq;
		const int32 XPForThisLevel = NewXP - PrevLevelUpReq;

		const float XPBarPercent = static_cast<float>(XPForThisLevel)/ static_cast<float>(DeltaLevelReq);

		OnXPPercentChangeDelegate.Broadcast(XPBarPercent);
	}
}

void UOverlayController::OnAbilityActivated(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PrevSlot) const
{
	const FAOGameplayTags& GameplayTags = FAOGameplayTags::Get();
	FAOAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.ability_status_unlocked;
	LastSlotInfo.InputTag = PrevSlot;
	LastSlotInfo.AbilityTag = GameplayTags.ability_none;
	//broadcast empty info if prev slot is valid slot when activating a given ability
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	FAOAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = Status;
	Info.InputTag = Slot;
	AbilityInfoDelegate.Broadcast(Info);
}

/*const FString Msg = FString::Printf(TEXT("GE Tag: %s"), *Tag.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Green, Msg);*/