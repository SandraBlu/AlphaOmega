// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Controllers/AbilityMenuController.h"
#include "AbilitySystem/AOAbilityComp.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Framework/AOPlayerState.h"
#include "AOGameplayTags.h"

void UAbilityMenuController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	AbilityPointsChanged.Broadcast(GetAOPS()->GetAbilityPts());
	
}

void UAbilityMenuController::BindCallbacksToDependencies()
{
	GetAOASC()->AbilityStatusChange.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 NewLevel)
	{
		if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
		{
			SelectedAbility.Status = StatusTag;

			bool bEnableSpendPoints = false;
			bool bEnableActivate = false;
			ShouldEnableButtons(StatusTag, CurrentAbilityPoints, bEnableSpendPoints, bEnableActivate);
			FString Description;
			FString NextLevelDescription;
			GetAOASC()->GetDescriptionsByAbilityType(AbilityTag, Description, NextLevelDescription);
			AbilitySelectedDelegate.Broadcast(bEnableSpendPoints, bEnableActivate, Description, NextLevelDescription);
		}
		if (AbilityInfo)
		{
			FAOAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	});

	GetAOASC()->AbilityActivated.AddUObject(this, &UAbilityMenuController::OnAbilityActivated);
	
	GetAOPS()->OnAbilityPtsChangeDelegate.AddLambda([this](int32 AbilityPoints)
	{
		AbilityPointsChanged.Broadcast(AbilityPoints);
		CurrentAbilityPoints = AbilityPoints;

		bool bEnableSpendPoints = false;
		bool bEnableActivate = false;
		ShouldEnableButtons(SelectedAbility.Status, CurrentAbilityPoints, bEnableSpendPoints, bEnableActivate);
		FString Description;
		FString NextLevelDescription;
		GetAOASC()->GetDescriptionsByAbilityType(SelectedAbility.Ability, Description, NextLevelDescription);
		AbilitySelectedDelegate.Broadcast(bEnableSpendPoints, bEnableActivate, Description, NextLevelDescription);
	});
}

void UAbilityMenuController::AbilitySelected(const FGameplayTag& AbilityTag)
{
	if (bWaitForActivateSelection)
	{
		const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType;
		StopWaitActivateDelegate.Broadcast(SelectedAbilityType);
		bWaitForActivateSelection = false;
	}
	
	const FAOGameplayTags GameplayTags = FAOGameplayTags::Get();
	const int32 AbilityPoints = GetAOPS()->GetAbilityPts();
	FGameplayTag AbilityStatus;
	
	const bool bTagValid = AbilityTag.IsValid();
	const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.ability_none);
	const FGameplayAbilitySpec* AbilitySpec = GetAOASC()->GetSpecFromAbilityTag(AbilityTag);
	const bool bSpecValid = AbilitySpec != nullptr;
	if (!bTagValid || bTagNone || !bSpecValid)
	{
		AbilityStatus = GameplayTags.ability_status_locked;
	}
	else
	{
		AbilityStatus = GetAOASC()->GetStatusTagFromSpec(*AbilitySpec);
	}

	SelectedAbility.Ability = AbilityTag;
	SelectedAbility.Status = AbilityStatus;
	bool bEnableSpendPoints = false;
	bool bEnableActivate = false;
	ShouldEnableButtons(AbilityStatus, AbilityPoints, bEnableSpendPoints, bEnableActivate);
	FString Description;
	FString NextLevelDescription;
	GetAOASC()->GetDescriptionsByAbilityType(AbilityTag, Description, NextLevelDescription);
	AbilitySelectedDelegate.Broadcast(bEnableSpendPoints, bEnableActivate, Description, NextLevelDescription);
}

void UAbilityMenuController::SpendPoint()
{
	if (GetAOASC())
	{
		GetAOASC()->SpendAbilityPoint(SelectedAbility.Ability);
	}
}

void UAbilityMenuController::AbilityDeselect()
{
	if (bWaitForActivateSelection)
	{
		FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
		StopWaitActivateDelegate.Broadcast(SelectedAbilityType);
		bWaitForActivateSelection = false;
	}
	
	SelectedAbility.Ability = FAOGameplayTags::Get().ability_none;
	SelectedAbility.Status = FAOGameplayTags::Get().ability_status_locked;

	AbilitySelectedDelegate.Broadcast(false, false, FString(), FString());
}

void UAbilityMenuController::ActivateButtonPressed()
{
	const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	WaitActivateDelegate.Broadcast(AbilityType);
	bWaitForActivateSelection = true;

	const FGameplayTag InputStatus = GetAOASC()->GetStatusFromAbilityTag(SelectedAbility.Ability);
	if (InputStatus.MatchesTagExact(FAOGameplayTags::Get().ability_status_active))
	{
		SelectedInputSlot = GetAOASC()->GetInputTagFromAbilityTag(SelectedAbility.Ability);
	}
}

void UAbilityMenuController::AbilityInputButtonPressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
	if (!bWaitForActivateSelection) return;
	// check selected ability against slot ability type. 
	const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(AbilityType)) return;

	//assign or reassign ability to new slot
	GetAOASC()->ActivateAbility(SelectedAbility.Ability, SlotTag);
}

void UAbilityMenuController::OnAbilityActivated(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PrevSlot)
{
	bWaitForActivateSelection = false;
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

	StopWaitActivateDelegate.Broadcast(AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType);
	AbilityReassigned.Broadcast(AbilityTag);
	AbilityDeselect();
}

void UAbilityMenuController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 AbilityPoints, bool& bShouldEnableSpendPointsButton, bool& bShouldEnableActivateButton)
{
	const FAOGameplayTags GameplayTags = FAOGameplayTags::Get();
	bShouldEnableSpendPointsButton = false;
	bShouldEnableActivateButton = false;
	if (AbilityStatus.MatchesTagExact(GameplayTags.ability_status_active))
	{
		bShouldEnableActivateButton = true;
		if (AbilityPoints > 0)
		{
			bShouldEnableSpendPointsButton = true;
		}
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.ability_status_available))
	{
		if (AbilityPoints > 0)
		{
			bShouldEnableSpendPointsButton = true;
		}
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.ability_status_unlocked))
	{
		bShouldEnableActivateButton = true;
		if (AbilityPoints > 0)
		{
			bShouldEnableSpendPointsButton = true;
		}
	}
}
