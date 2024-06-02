// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Controllers/AOAttributeMenuController.h"
#include "Attributes/AOAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Framework/AOPlayerState.h"
#include "AbilitySystem/AOAbilityComp.h"

void UAOAttributeMenuController::BindCallbacksToDependencies()
{
	
	for (auto& Pair : GetAOAS()->AttributeTagMap)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}
	
	GetAOPS()->OnAttributePtsChangeDelegate.AddLambda([this](int32 Points)
		{
			AttributePtsChangeDelegate.Broadcast(Points);
		}
		);
}

void UAOAttributeMenuController::BroadcastInitialValues()
{
	
	check(AttributeInfo)
		for (auto& Pair : GetAOAS()->AttributeTagMap)
		{
			BroadcastAttributeInfo(Pair.Key, Pair.Value());
			
		}
		
		AttributePtsChangeDelegate.Broadcast(GetAOPS()->GetAttributePts());
}

void UAOAttributeMenuController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	GetAOASC()->UpgradeAttribute(AttributeTag);
}

void UAOAttributeMenuController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	FAOAttributeData Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
