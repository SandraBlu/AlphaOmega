// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "OGameplayAbility.generated.h"

class UOAbilitySystemComponent;
class APlayerCharacter;
/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UOGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FGameplayTag StartupInputTag;

	virtual FString GetDescription(int32 Level);
	virtual FString GetNextLevelDescription(int32 Level);
	static FString GetLockedDescription(int32 Level);

protected:

	float GetEnergyCost(float InLevel = 1.f) const;
	float GetCooldown(float InLevel = 1.f) const;

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UOAbilitySystemComponent* GetRAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	APlayerCharacter* GetPlayerFromActorInfo();
	
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	virtual class ANPCBase* GetEnemyCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	virtual class AOPlayerController* GetPlayerControllerFromActorInfo();

private:
	
	TWeakObjectPtr<APlayerCharacter> CachedPlayer;
	TWeakObjectPtr<ANPCBase> CachedEnemy;
	TWeakObjectPtr<AOPlayerController> CachedPlayerController;
	
};
