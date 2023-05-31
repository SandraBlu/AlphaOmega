// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/AOTargetType.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"

void UAOTargetType::GetTargets_Implementation(AAOCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	return;
}

void UAOTargetType_UseOwner::GetTargets_Implementation(AAOCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	OutActors.Add(TargetingCharacter);
}

void UAOTargetType_UseEventData::GetTargets_Implementation(AAOCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	const FHitResult* FoundHitResult = EventData.ContextHandle.GetHitResult();
	const FHitResult TargetDataHitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(EventData.TargetData, 0);
	if (FoundHitResult)
	{
		OutHitResults.Add(*FoundHitResult);
	}
	else if (TargetDataHitResult.IsValidBlockingHit())
	{
		OutHitResults.Add(TargetDataHitResult);
	}
	else if (EventData.Target)
	{
		const AActor* Actor = EventData.Target;
		OutActors.Add(const_cast<AActor*>(Actor));
	}
}


