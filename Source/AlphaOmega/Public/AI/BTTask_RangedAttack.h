// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RangedAttack.generated.h"

class UAOCoreComponent;

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UBTTask_RangedAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_RangedAttack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	protected:

	UPROPERTY(EditAnywhere, Category = "AI")
	float MaxProjectileSpread;

	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<AActor> ProjectileClass;

};
