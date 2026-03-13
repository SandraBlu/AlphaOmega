// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_OnTick.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityTaskTickDelegate,float,DeltaTime);
/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UAbilityTask_OnTick : public UAbilityTask
{
	GENERATED_BODY()

public:

	UAbilityTask_OnTick();

	UFUNCTION(BlueprintCallable, Category = "Warrior|AbilityTasks", meta = (HidePin = "OwningAbility",DefaultToSelf = "OwningAbility",BlueprintInternalUseOnly = "true"))
	static UAbilityTask_OnTick* ExecuteTaskOnTick(UGameplayAbility* OwningAbility);
	
	virtual void TickTask(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FOnAbilityTaskTickDelegate OnAbilityTaskTick;
	
};
