// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Input/InputConfig.h"
#include "OInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UOInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	
	template<class UserClass, typename CallBackFunc>
	void BindAbilityActions(const UInputConfig* InputConfig, UserClass* Object, CallBackFunc PressedFunc, CallBackFunc  ReleasedFunc);
	
};

template <class UserClass, typename CallBackFunc>
void UOInputComponent::BindAbilityActions(const UInputConfig* InputConfig, UserClass* Object, CallBackFunc PressedFunc, CallBackFunc ReleasedFunc)
{
	check(InputConfig);

	for (const FOInputAction& Action : InputConfig->AbilityInput)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
		}
	}
}