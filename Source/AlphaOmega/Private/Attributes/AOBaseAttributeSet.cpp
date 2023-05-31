// Fill out your copyright notice in the Description page of Project Settings.


#include "Attributes/AOBaseAttributeSet.h"
#include "Framework/AOBFL.h"
#include "Components/AOCoreComponent.h"

UAOBaseAttributeSet::UAOBaseAttributeSet()
{

}

void UAOBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// This is called whenever attributes change, so for max attributes we want to scale the current totals to match
	Super::PreAttributeChange(Attribute, NewValue);

	const FGameplayAbilityActorInfo* ActorInfo = GetActorInfo();
	if (!ActorInfo)
	{
		return;
	}

	const TWeakObjectPtr<AActor> AvatarActorPtr = ActorInfo->AvatarActor;
	if (!AvatarActorPtr.IsValid())
	{
		return;
	}

	const AActor* AvatarActor = AvatarActorPtr.Get();
	UAOCoreComponent* CoreComponent = UAOBFL::GetAOCoreComponent(AvatarActor);
 	if (CoreComponent)
 	{
 		CoreComponent->PreAttributeChange(this, Attribute, NewValue);
 	}
}

void UAOBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	AActor* Instigator = nullptr;
	AActor* TargetActor = nullptr;
	GetSourceAndTargetFromContext<AActor>(Data, Instigator, TargetActor);

	UAOCoreComponent* TargetCoreComponent = UAOBFL::GetAOCoreComponent(TargetActor);
	if (TargetCoreComponent)
	{
		TargetCoreComponent->PostGameplayEffectExecute(this, Data);
	}
}

float UAOBaseAttributeSet::GetClampMinimumValueFor(const FGameplayAttribute& Attribute)
{
	return 0.f;
}

const FGameplayTagContainer& UAOBaseAttributeSet::GetSourceTagsFromContext(const FGameplayEffectModCallbackData& Data)
{
	return *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
}

void UAOBaseAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty) const
{
	UAbilitySystemComponent* AbilitySystemComponent = GetOwningAbilitySystemComponent();
	if (!AbilitySystemComponent)
	{
		return;
	}

	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	const float CurrentValue = AffectedAttribute.GetCurrentValue();

	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && CurrentMaxValue > 0.f)
	{
		// Get the current relative percent
		const float Ratio = CurrentValue / CurrentMaxValue;

		// Calculate value for the affected attribute based on current ratio
		const float NewValue = FMath::RoundToFloat(NewMaxValue * Ratio);

		AbilitySystemComponent->ApplyModToAttribute(AffectedAttributeProperty, EGameplayModOp::Override, NewValue);
	}
}

void UAOBaseAttributeSet::GetExecutionDataFromMod(const FGameplayEffectModCallbackData& Data, OUT FAttributeExecutionData& OutExecutionData)
{
	OutExecutionData.Context = Data.EffectSpec.GetContext();
	OutExecutionData.SourceASC = OutExecutionData.Context.GetOriginalInstigatorAbilitySystemComponent();
	OutExecutionData.SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	Data.EffectSpec.GetAllAssetTags(OutExecutionData.SpecAssetTags);

	OutExecutionData.TargetActor = Data.Target.AbilityActorInfo->AvatarActor.IsValid() ? Data.Target.AbilityActorInfo->AvatarActor.Get() : nullptr;
	OutExecutionData.TargetController = Data.Target.AbilityActorInfo->PlayerController.IsValid() ? Data.Target.AbilityActorInfo->PlayerController.Get() : nullptr;
	OutExecutionData.TargetPawn = Cast<APawn>(OutExecutionData.TargetActor);
	OutExecutionData.TargetCoreComponent = UAOBFL::GetAOCoreComponent(OutExecutionData.TargetActor);

	if (OutExecutionData.SourceASC && OutExecutionData.SourceASC->AbilityActorInfo.IsValid())
	{
		// Get the Source actor, which should be the damage causer (instigator)
		if (OutExecutionData.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
		{
			// Set the source actor based on context if it's set
			if (OutExecutionData.Context.GetEffectCauser())
			{
				OutExecutionData.Instigator = OutExecutionData.Context.GetEffectCauser();
			}
			else
			{
				OutExecutionData.Instigator = OutExecutionData.SourceASC->AbilityActorInfo->AvatarActor.IsValid()
					? OutExecutionData.SourceASC->AbilityActorInfo->AvatarActor.Get()
					: nullptr;
			}
		}

		OutExecutionData.SourceController = OutExecutionData.SourceASC->AbilityActorInfo->PlayerController.IsValid()
			? OutExecutionData.SourceASC->AbilityActorInfo->PlayerController.Get()
			: nullptr;
		OutExecutionData.SourcePawn = Cast<APawn>(OutExecutionData.Instigator);
		OutExecutionData.SourceCoreComponent = UAOBFL::GetAOCoreComponent(OutExecutionData.Instigator);
	}

	OutExecutionData.SourceObject = Data.EffectSpec.GetEffectContext().GetSourceObject();

	// Compute the delta between old and new, if it is available
	OutExecutionData.DeltaValue = 0.f;
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
	{
		// If this was additive, store the raw delta value to be passed along later
		OutExecutionData.DeltaValue = Data.EvaluatedData.Magnitude;
	}
}
