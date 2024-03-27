// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Attributes/AOAttributeSet.h"
#include "AOCharacter.generated.h"

class UAOAttributeSet;

UCLASS()
class ALPHAOMEGA_API AAOCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemComp;
	
	UPROPERTY()
	UAOAttributeSet* Attributes;

public:
	
	AAOCharacter();

	// Overridden from Ability System Interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void InitializeAttributes();
	virtual void GrantAbilities();

	/** List of GameplayEffects to apply when the Ability System Component is initialized (typically on begin play) */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> GrantedEffect;

	/** List of GameplayEffects to apply when the Ability System Component is initialized (typically on begin play) */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> GrantedAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UAOInventoryComponent* PlayerInventory;

	UFUNCTION(BlueprintCallable)
	void SetLootSource(class UAOInventoryComponent* NewLootSource);

	
 	UFUNCTION(BlueprintPure, Category = "Loot")
	bool IsLooting() const;

 	UFUNCTION(BlueprintCallable, Category = "Loot")
	void LootItem(class UAOItem* ItemToGive);

protected:

	//Looting--------------------------------------------
	
	UFUNCTION()
 	void BeginLootingNPC(class AAOCharacter* Character);

	//Inventory we are currently looting from
	UPROPERTY(BlueprintReadOnly)
	UAOInventoryComponent* LootSource;
	
 	UFUNCTION()
 	void OnLootSourceDestroyed(AActor* DestroyedActor);
 	
 	UFUNCTION()
 	void ShowHideLootMenu();

};
