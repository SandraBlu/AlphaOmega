// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Abilities/AOGameplayAbility.h"
#include "AOTargetType.generated.h"


struct FGameplayEventData;
class AAOCharacter;
class AActor;

/**
 * 
 */
UCLASS(Blueprintable, meta = (ShowWorldContextPin))
class ALPHAOMEGA_API UAOTargetType : public UObject
{
	GENERATED_BODY()
	
		public:
	// Constructor and overrides
	UAOTargetType() {}

	/** Called to determine targets to apply gameplay effects to */
	UFUNCTION(BlueprintNativeEvent)
    void GetTargets(AAOCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const;

};

//Class that uses owner
UCLASS(NotBlueprintable)
class ALPHAOMEGA_API UAOTargetType_UseOwner : public UAOTargetType
{
	GENERATED_BODY()
	
public:
	
	UAOTargetType_UseOwner() {}

	//uses passed in event data
	
    virtual void GetTargets_Implementation(AAOCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;

};

//Class that uses owner
UCLASS(NotBlueprintable)
class ALPHAOMEGA_API UAOTargetType_UseEventData : public UAOTargetType
{
	GENERATED_BODY()
	
public:
	
	UAOTargetType_UseEventData() {}

	//uses passed in event data
	
    virtual void GetTargets_Implementation(AAOCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;

};