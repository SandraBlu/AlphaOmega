// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/InputConfig.h"

const UInputAction* UInputConfig::FindAbilityInputForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FOInputAction& Action : AbilityInput)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("No AbilityInput for InputTag [%s], on InputConfig [%s]"), *InputTag.ToString(), *GetNameSafe(this));
	}
	return nullptr;
}
