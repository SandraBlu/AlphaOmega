// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AOGameplayAbility.generated.h"

class UAbilitySystemComponent;
class UWorld;
class UAOTargetType;

USTRUCT(BlueprintType)
struct FAOGameplayEffectContainer
{
	GENERATED_BODY()

public:
	FAOGameplayEffectContainer() {}

	/** Sets the way that targeting happens */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TSubclassOf<UAOTargetType> TargetType;

	/** List of gameplay effects to apply to the targets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TArray<TSubclassOf<UGameplayEffect>> TargetGameplayEffectClasses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	bool bUseSetByCallerMagnitude = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	FGameplayTag SetByCallerDataTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	float SetByCallerMagnitude = 1.0f;
};

/** A "processed" version of GameplayEffectContainer that can be passed around and eventually applied */
USTRUCT(BlueprintType)
struct  FAOGameplayEffectContainerSpec
{
	GENERATED_BODY()

public:
	FAOGameplayEffectContainerSpec() {}
	/** Computed target data */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	FGameplayAbilityTargetDataHandle TargetData;

	/** List of gameplay effects to apply to the targets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TArray<FGameplayEffectSpecHandle> TargetGameplayEffectSpecs;

	/** Returns true if this has any valid effect specs */
	bool HasValidEffects();

	/** Returns true if this has any valid targets */
	bool HasValidTargets();

	/** Adds new targets to target data */
	void AddTargets(const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);
};
/**
 * 
 */
UCLASS(Blueprintable)
class ALPHAOMEGA_API UAOGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:

	UAOGameplayAbility();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameplayEffects)
	TMap<FGameplayTag, FAOGameplayEffectContainer> EffectContainerMap;


	//Start immediately when added to ability component
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	bool bAutoStart;

	//Action Nickname to start/stop without a ref to Object
	UPROPERTY(EditAnywhere, Category = "Ability")
	FName AbilityName;

	 /** Make gameplay effect container spec to be applied later, using the passed in container */
    UFUNCTION(BlueprintCallable, Category = "GAS Ability", meta=(AutoCreateRefTerm = "EventData"))
    virtual FAOGameplayEffectContainerSpec MakeEffectContainerSpecFromContainer(const FAOGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

    /** Search for and make a gameplay effect container spec to be applied later, from the EffectContainerMap */
    UFUNCTION(BlueprintCallable, Category = "GAS Abilityy", meta = (AutoCreateRefTerm = "EventData"))
    virtual FAOGameplayEffectContainerSpec MakeEffectContainerSpec(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

    /** Applies a gameplay effect container spec that was previously created */
    UFUNCTION(BlueprintCallable, Category = "GAS Ability")
    virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainerSpec(const FAOGameplayEffectContainerSpec& ContainerSpec);

    /** Applies a gameplay effect container, by creating and then applying the spec */
    UFUNCTION(BlueprintCallable, Category = "GAS Ability", meta = (AutoCreateRefTerm = "EventData"))
    virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainer(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

protected:

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UTexture2D* Icon;
};
