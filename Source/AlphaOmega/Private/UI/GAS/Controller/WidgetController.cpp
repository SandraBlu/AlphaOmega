// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GAS/Controller/WidgetController.h"

#include "Framework/OPlayerController.h"
#include "Framework/OPlayerState.h"
#include "GAS/OAbilitySystemComponent.h"
#include "GAS/OAttributeSet.h"
#include "GAS/Data/AbilityInfo.h"

void UWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UWidgetController::BroadcastInitialValues()
{
}

void UWidgetController::BindCallbacksToDependencies()
{
}

void UWidgetController::BroadcastAbilityInfo()
{
	if (!GetASC()->bGrantedAbilitiesGiven) return;
	
	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		FOAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(OASC->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = OASC->GetInputTagFromSpec(AbilitySpec);
		Info.StatusTag = OASC->GetStatusFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	});
	GetASC()->ForEachAbility(BroadcastDelegate);
}

AOPlayerController* UWidgetController::GetPC()
{
	if (OPC == nullptr)
	{
		OPC = Cast<AOPlayerController>(PlayerController);
	}
	return OPC;
}

AOPlayerState* UWidgetController::GetPS()
{
	if (OPS == nullptr)
	{
		OPS = Cast<AOPlayerState>(PlayerState);
	}
	return OPS;
}

UOAbilitySystemComponent* UWidgetController::GetASC()
{
	if (OASC == nullptr)
	{
		OASC = Cast<UOAbilitySystemComponent>(AbilitySystemComponent);
	}
	return OASC;
}

UOAttributeSet* UWidgetController::GetAS()
{
	if (OAS == nullptr)
	{
		OAS = Cast<UOAttributeSet>(AttributeSet);
	}
	return OAS;
}