// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/CombatInterface.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "FX/DebuffNiagaraComp.h"
#include "AOCharacter.generated.h"

class UAOFootstepsComponent;
class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;
class AAOWeapon;
class UAnimMontage;
class UNiagaraSystem;
class UAOInventoryComponent;

UCLASS()
class ALPHAOMEGA_API AAOCharacter : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	
	AAOCharacter();

	// Overridden from Ability System Interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() { return AttributeSet; }

	//Footsteps Comp getter-----------------
	UAOFootstepsComponent* GetFootstepsComp() const;

	//Combat Interface
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& CombatSocketTag) override;
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual void Die() override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation();
	virtual UNiagaraSystem* GetBloodEffect_Implementation();
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag);
	virtual ECharacterClass GetCharacterClass_Implementation() override;
	virtual FOnDeath GetOnDeathDelegate() override;
	//Combat Interface

	FOnDeath  OnDeath;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;

protected:

	virtual void InitAbilityActorInfo();
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass, float Level) const;
	virtual void InitializeAttributes() const;
	void GrantAbilities();

	UPROPERTY()
	UAttributeSet* AttributeSet;

	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<class UGameplayEffect> BaseAttributes;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<class UGameplayEffect> PrimaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<class UGameplayEffect> SecondaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<class UGameplayEffect> ResistanceAttributes;

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> GrantedAbilities;

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> PassiveAbilities;

	//Combat Interface
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XP")
	int32 CharacterLevel = 1;

	//Combat
	UPROPERTY(EditAnywhere, Category = "Combat")
	AAOWeapon* EquippedWeapon;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName RHand;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName LHand;

	bool bDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UNiagaraSystem* BloodEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	USoundBase* DeathSFX;

	UFUNCTION(BlueprintPure)
	FORCEINLINE AAOWeapon* GetEquippedWeapon() const { return EquippedWeapon; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default Settings")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	UDebuffNiagaraComp* NiagaraDebuffComp;

	//Footsteps Component
	UPROPERTY(BlueprintReadOnly)
	UAOFootstepsComponent* FootstepsComp;

	class UAOAbilityComp* AOASC;

	class UAOAbilityComp* GetASC();

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* HitReactMontage;

};
