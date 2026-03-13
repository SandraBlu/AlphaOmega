// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GAS/Controller/AttributeMenuController.h"

#include "Framework/OPlayerState.h"
#include "GAS/OAbilitySystemComponent.h"
#include "GAS/OAttributeSet.h"
#include "GAS/Data/AttributeInfo.h"

void UAttributeMenuController::BroadcastInitialValues()
{
	check(AttributeInfo);
	for (auto& Pair : GetAS()->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
	AttributePointsChangeDelegate.Broadcast(GetPS()->GetAttributePts());
}

void UAttributeMenuController::BindCallbacksToDependencies()
{
	check(AttributeInfo);
	for (auto& Pair : GetAS()->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda([this, Pair](const FOnAttributeChangeData& Data)
		{
			BroadcastAttributeInfo(Pair.Key, Pair.Value());
		});	
	}
	GetPS()->OnAttributePtsChangeDelegate.AddLambda([this] (int32 Points)
	{
		AttributePointsChangeDelegate.Broadcast(Points);
	});
}

void UAttributeMenuController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	GetASC()->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,const FGameplayAttribute& Attribute) const
{
	FAttributeData Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
