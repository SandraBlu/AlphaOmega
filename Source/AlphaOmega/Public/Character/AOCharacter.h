// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Attributes/AOPlayerAttributeSet.h"
#include "AOCharacter.generated.h"


class UAOAbilitySystemComponent;
class UPlayerAttributeSet;


UCLASS()
class ALPHAOMEGA_API AAOCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemComp;
	
	UPROPERTY()
	UAOPlayerAttributeSet* Attributes;

public:
	

	AAOCharacter();

	// Overridden from Ability System Interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void InitializeAttributes();
	virtual void GrantAbilities();

	/** List of GameplayEffects to apply when the Ability System Component is initialized (typically on begin play) */
	UPROPERTY(EditDefaultsOnly, Category = "AOAbilities")
		TSubclassOf<class UGameplayEffect> GrantedEffect;

	/** List of GameplayEffects to apply when the Ability System Component is initialized (typically on begin play) */
	UPROPERTY(EditDefaultsOnly, Category = "AOAbilities")
		TArray<TSubclassOf<class UGameplayAbility>> GrantedAbilities;
	
	//ChangeAbility Level With Tag
	UFUNCTION(BlueprintCallable, Category = "AOAbilities")
	void ChangeAbilityLevelWithTags(FGameplayTagContainer TagContainer, int32 NewLevel);

	//Apply GE To TargetData

	UFUNCTION(BlueprintCallable, Category = "TargetData")
	void ApplyGEtoTarget(const FGameplayEffectSpecHandle& GESpec, const FGameplayAbilityTargetDataHandle& TargetData);


};
