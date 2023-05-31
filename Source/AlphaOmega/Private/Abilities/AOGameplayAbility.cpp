// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/AOGameplayAbility.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/AOTargetType.h"

UAOGameplayAbility::UAOGameplayAbility()
{
	
}

FAOGameplayEffectContainerSpec UAOGameplayAbility::MakeEffectContainerSpecFromContainer(const FAOGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel /*= -1*/)
{
	// First figure out our actor info
	FAOGameplayEffectContainerSpec ReturnSpec;
	const AActor* OwningActor = GetOwningActorFromActorInfo();
	UAbilitySystemComponent* OwningASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor);

	if (OwningASC)
	{
		// If we have a target type, run the targeting logic. This is optional, targets can be added later
		if (Container.TargetType.Get())
		{
			TArray<FHitResult> HitResults;
			TArray<AActor*> TargetActors;
			const UAOTargetType* TargetType = Container.TargetType.GetDefaultObject();
			AActor* AvatarActor = GetAvatarActorFromActorInfo();
			//TargetType->GetTargets(AvatarActor, EventData, HitResults, TargetActors);
			ReturnSpec.AddTargets(HitResults, TargetActors);
		}

		// If we don't have an override level, use the default on the ability itself
		if (OverrideGameplayLevel == INDEX_NONE)
		{
			OverrideGameplayLevel = GetAbilityLevel();
		}

		// Build GameplayEffectSpecs for each applied effect
		for (const TSubclassOf<UGameplayEffect>& EffectClass : Container.TargetGameplayEffectClasses)
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(EffectClass, OverrideGameplayLevel);

			FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
			if (Spec && Container.bUseSetByCallerMagnitude)
			{
				Spec->SetSetByCallerMagnitude(Container.SetByCallerDataTag, Container.SetByCallerMagnitude);
			}
			ReturnSpec.TargetGameplayEffectSpecs.Add(SpecHandle);
		}
	}
	return ReturnSpec;
}

FAOGameplayEffectContainerSpec UAOGameplayAbility::MakeEffectContainerSpec(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel /*= -1*/)
{
	FAOGameplayEffectContainer* FoundContainer = EffectContainerMap.Find(ContainerTag);

	if (FoundContainer)
	{
		return MakeEffectContainerSpecFromContainer(*FoundContainer, EventData, OverrideGameplayLevel);
	}
	return FAOGameplayEffectContainerSpec();
}

TArray<FActiveGameplayEffectHandle> UAOGameplayAbility::ApplyEffectContainerSpec(const FAOGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	// Iterate list of effect specs and apply them to their target data
	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		AllEffects.Append(K2_ApplyGameplayEffectSpecToTarget(SpecHandle, ContainerSpec.TargetData));
	}
	return AllEffects;
}

TArray<FActiveGameplayEffectHandle> UAOGameplayAbility::ApplyEffectContainer(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel /*= -1*/)
{
	const FAOGameplayEffectContainerSpec Spec = MakeEffectContainerSpec(ContainerTag, EventData, OverrideGameplayLevel);
	return ApplyEffectContainerSpec(Spec);
}

bool FAOGameplayEffectContainerSpec::HasValidEffects()
{
	return TargetGameplayEffectSpecs.Num() > 0;
}

bool FAOGameplayEffectContainerSpec::HasValidTargets()
{
	return TargetData.Num() > 0;
}

void FAOGameplayEffectContainerSpec::AddTargets(const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors)
{
	for (const FHitResult& HitResult : HitResults)
	{
		FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		TargetData.Add(NewData);
	}

	if (TargetActors.Num() > 0)
	{
		FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
		NewData->TargetActorArray.Append(TargetActors);
		TargetData.Add(NewData);
	}
}
