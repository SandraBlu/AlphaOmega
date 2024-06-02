// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Controllers/AOWidgetController.h"
#include "Framework/AOPlayerState.h"
#include "Framework/AOPlayerController.h"
#include "AbilitySystem/AOAbilityComp.h"
#include "Attributes/AOAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UAOWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UAOWidgetController::BroadcastInitialValues()
{

}

void UAOWidgetController::BindCallbacksToDependencies()
{

}

void UAOWidgetController::BroadcastAbilityInfo()
{
	if (!GetAOASC()->bAbilityGranted) return;

	FCheckForAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
		{

			FAOAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AOAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
			Info.InputTag = AOAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);
			Info.StatusTag = AOAbilitySystemComponent->GetStatusTagFromSpec(AbilitySpec);
			AbilityInfoDelegate.Broadcast(Info);
		});
	GetAOASC()->CheckForAbility(BroadcastDelegate);
}

AAOPlayerController* UAOWidgetController::GetAOPC()
{
	if (AOPlayerController == nullptr)
	{
		AOPlayerController = Cast<AAOPlayerController>(PlayerController);
	}
	return AOPlayerController;
}

AAOPlayerState* UAOWidgetController::GetAOPS()
{
	if (AOPlayerState == nullptr)
	{
		AOPlayerState = Cast<AAOPlayerState>(PlayerState);
	}
	return AOPlayerState;
}

UAOAbilityComp* UAOWidgetController::GetAOASC()
{
	if (AOAbilitySystemComponent == nullptr)
	{
		AOAbilitySystemComponent = Cast<UAOAbilityComp>(AbilitySystemComponent);
	}
	return AOAbilitySystemComponent;
}

UAOAttributeSet* UAOWidgetController::GetAOAS()
{
	if (AOAttributeSet == nullptr)
	{
		AOAttributeSet
			= Cast<UAOAttributeSet>(AttributeSet);
	}
	return 	AOAttributeSet;
}
