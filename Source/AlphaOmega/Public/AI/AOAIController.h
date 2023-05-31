// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AOAIController.generated.h"

class UBehaviorTree;

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API AAOAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* BTree;

	virtual void  BeginPlay() override;
};
