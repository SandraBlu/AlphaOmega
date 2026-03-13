// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AsyncTask/AbilityTask_OnTick.h"

UAbilityTask_OnTick::UAbilityTask_OnTick()
{
	bTickingTask = true;
}

UAbilityTask_OnTick* UAbilityTask_OnTick::ExecuteTaskOnTick(UGameplayAbility* OwningAbility)
{
	UAbilityTask_OnTick* TaskNode = NewAbilityTask<UAbilityTask_OnTick>(OwningAbility);

	return TaskNode;
}

void UAbilityTask_OnTick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnAbilityTaskTick.Broadcast(DeltaTime);
	}
	else
	{
		EndTask();
	}
}
