// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Framework/InputDataAsset.h"
#include "AOInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UAOInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
	void BindAbilityActions(const UInputDataAsset* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType  ReleasedFunc, HeldFuncType HeldFunc);
	
};

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
inline void UAOInputComponent::BindAbilityActions(const UInputDataAsset* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);

	for (const FAOInputAction& Action : InputConfig->AbilityInput)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}
			if (ReleasedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
			}
			if (HeldFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
			}
		}
	}
}
