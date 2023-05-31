// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "AOBaseAttributeSet.generated.h"

class UAOCoreComponent;

/** Structure holding various information to deal with AttributeSet PostGameplayEffectExecute, extracting info from FGameplayEffectModCallbackData */
USTRUCT()
struct FAttributeExecutionData
{
	GENERATED_BODY()

	/** The physical representation of the Source ASC (The ability system component of the instigator that started the whole chain) */
	UPROPERTY()
	AActor* Instigator = nullptr;

	/** The physical representation of the owner (Avatar) for the target we intend to apply to  */
	UPROPERTY()
	AActor* TargetActor = nullptr;

	/** The ability system component of the instigator that started the whole chain */
	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;

	//Core actor component attached to Source Actor (if any) */
	//UPROPERTY()
	UAOCoreComponent* SourceCoreComponent = nullptr;

	//Core actor component attached to Target Actor (if any) */
	UPROPERTY()
	UAOCoreComponent* TargetCoreComponent = nullptr;

	/** PlayerController associated with the owning actor for the Source ASC (The ability system component of the instigator that started the whole chain) */
	UPROPERTY()
	APlayerController* SourceController = nullptr;

	/** PlayerController associated with the owning actor for the target we intend to apply to */
	UPROPERTY()
	APlayerController* TargetController = nullptr;

	/** The physical representation of the Source ASC (The ability system component of the instigator that started the whole chain), as a APawn */
	UPROPERTY()
	APawn* SourcePawn = nullptr;

	/** The physical representation of the owner (Avatar) for the target we intend to apply to, as a APawn */
	UPROPERTY()
	APawn* TargetPawn = nullptr;

	/** The object this effect was created from. */
	UPROPERTY()
	UObject* SourceObject = nullptr;

	/** This tells us how we got here (who / what applied us) */
	FGameplayEffectContextHandle Context;

	/** Combination of spec and actor tags for the captured Source Tags on GameplayEffectSpec creation */
	FGameplayTagContainer SourceTags;

	/** All tags that apply to the gameplay effect spec */
	FGameplayTagContainer SpecAssetTags;

	/** Holds the delta value between old and new, if it is available (for Additive Operations) */
	float DeltaValue;
};

	// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UAOBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	// Sets default values for this AttributeSet attributes
	UAOBaseAttributeSet();

	// AttributeSet Overrides
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	/** Helper function to get the minimum clamp value for a given attribute. Subclasses are expected to override this. */
	virtual float GetClampMinimumValueFor(const FGameplayAttribute& Attribute);

	/**
	 * Returns the aggregated SourceTags for this EffectSpec
	 */
	virtual const FGameplayTagContainer& GetSourceTagsFromContext(const FGameplayEffectModCallbackData& Data);

	/**
	 * Templated version of GetCharactersFromContext
	 */
	template <class TReturnType>
	void GetSourceAndTargetFromContext(const FGameplayEffectModCallbackData& Data, TReturnType*& Instigator, TReturnType*& TargetActor)
	{
		const FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
		UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();

		// Get the Target actor, which should be our owner
		if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
		{
			TargetActor = Cast<TReturnType>(Data.Target.AbilityActorInfo->AvatarActor.Get());
		}

		// Get the Source actor, which should be the damage causer (instigator)
		if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
		{
			// Set the source actor based on context if it's set
			if (Context.GetEffectCauser())
			{
				Instigator = Cast<TReturnType>(Context.GetEffectCauser());
			}
			else
			{
				Instigator = Cast<TReturnType>(Source->AbilityActorInfo->AvatarActor.Get());
			}
		}
	}

	/**
	 * Helper function to proportionally adjust the value of an attribute when it's associated max attribute changes.
	 *
	 * (e.g. When MaxHealth increases, Health increases by an amount that maintains the same percentage as before)
	 */
	virtual void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty) const;

protected:

	// Fills out FAttributeExecutionData structure based on provided data.
	virtual void GetExecutionDataFromMod(const FGameplayEffectModCallbackData& Data, OUT FAttributeExecutionData& OutExecutionData);
};