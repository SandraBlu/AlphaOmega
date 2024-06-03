// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AOCharacter.h"
#include "Interfaces/CombatInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "UI/Controllers/OverlayController.h"
#include "AICharacter.generated.h"



class UWidgetComponent;
class UBehaviorTree;
class AAOAIController;

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API AAICharacter : public AAOCharacter
{
	GENERATED_BODY()

public:

	AAICharacter();

	//Combat Interface
	virtual int32 GetCharacterLevel_Implementation() override;
	virtual void Die() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	virtual void DirectionalHitReact(const FVector& ImpactPoint);
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& CombatSocketTag) override;
	//Combat Interface

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChange OnHealthChange;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChange OnMaxHealthChange;
	
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting = false;

	UFUNCTION(BlueprintImplementableEvent)
	void DissolveMesh();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	AActor* CombatTarget;



private:

	//Montage
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReact;

	UPROPERTY(EditAnywhere, Category = "SFX")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "VFX")
	UNiagaraSystem* HitParticles;

protected:
	
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeAttributes() const override;

	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default Settings")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* Health;

	UPROPERTY(EditAnywhere, Category = "Combat")
	USkeletalMeshComponent* Weapon;

	//Weapon Damage Socket
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName DamageSocket;

	//BT
	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BTree;

	UPROPERTY()
	AAOAIController* AIC;

	/* Widget to display when bot first sees a player. */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SpottedWidgetClass;

	/* Material parameter for Hitflashes */
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TimeToHitParamName;

	/* Key for AI Blackboard 'TargetActor' */
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TargetActorKey;

	UFUNCTION()
	void SetTargetActor(AActor* NewTarget);

	virtual void PostInitializeComponents();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UPawnSensingComponent* PawnSensingComp;

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	//Montage
	void PlayHitReactMontage(const FName& SectionName);
	
};
