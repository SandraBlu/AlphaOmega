// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameplayTagContainer.h"
#include "DebuffNiagaraComp.generated.h"

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UDebuffNiagaraComp : public UNiagaraComponent
{
	GENERATED_BODY()

public:

	UDebuffNiagaraComp();

	UPROPERTY(VisibleAnywhere)
	FGameplayTag DebuffTag;

	protected:

	virtual void BeginPlay() override;

	void DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION()
	void OnOwnerDeath(AActor* DeadActor);
};
