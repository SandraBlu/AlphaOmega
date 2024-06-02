// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/AOEffector.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AAOEffector::AAOEffector()
{
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(GetRootComponent());
}

void AAOEffector::ApplyEffectToTarget(AActor* TargetActor, const FEffectType& Effect)
{
	if (TargetActor->ActorHasTag(FName("enemy")) && !bApplyEffectToEnemy) return;

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr) return;

	check(Effect.GameplayEffect);
	FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpec = TargetASC->MakeOutgoingSpec(Effect.GameplayEffect, ActorLevel, EffectContext);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());

	if (EffectSpec.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite)
	{
		if (Effect.RemovalPolicy == ERemoveEffectPolicy::RemoveOnEndOverlap)
		{
			ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
		}
	}
	if (bDestroyOnEffectApplication)
	{
		Destroy();
	}
}

void AAOEffector::OnOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag(FName("enemy")) && !bApplyEffectToEnemy) return;

	for (const FEffectType& Effect : Effects)
	{
		if (Effect.ApplicationPolicy == EApplyEffectPolicy::ApplyOnOverlap)
		{
			ApplyEffectToTarget(TargetActor, Effect);
		}
	}
}

void AAOEffector::OnEndOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag(FName("enemy")) && !bApplyEffectToEnemy) return;

	for (const FEffectType& Effect : Effects)
	{
		if (Effect.ApplicationPolicy == EApplyEffectPolicy::ApplyOnEndOverlap)
		{
			ApplyEffectToTarget(TargetActor, Effect);
		}

		if (Effect.GameplayEffect.GetDefaultObject()->DurationPolicy == EGameplayEffectDurationType::Infinite)
		{
			if (Effect.RemovalPolicy == ERemoveEffectPolicy::RemoveOnEndOverlap)
			{
				UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

				if (!IsValid(TargetASC)) return;

				TArray<FActiveGameplayEffectHandle> HandlesToRemove;
				for (auto HandlePair : ActiveEffectHandles)
				{
					if (TargetASC == HandlePair.Value)
					{
						TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
						HandlesToRemove.Add(HandlePair.Key);
					}
				}

				for (FActiveGameplayEffectHandle& Handle : HandlesToRemove)
				{
					ActiveEffectHandles.FindAndRemoveChecked(Handle);
				}
			}
		}
	}
}

