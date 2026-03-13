// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "OAIController.generated.h"

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API AOAIController : public AAIController
{
	GENERATED_BODY()

public:
	
	AOAIController();

	virtual  ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	
protected:

	virtual void BeginPlay() override;

	UPROPERTY()
	UBehaviorTree* BTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Behavior Tree")
	class UAIPerceptionComponent* PerceptionComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Behavior Tree")
	class UAISenseConfig_Sight* AISightConfig;

	UFUNCTION()
	virtual void OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Crowd Avoidance Config")
	bool bEnableCrowdAvoidance = true;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crowd Avoidance Config", meta = (EditCondition = "bEnableCrowdAvoidance",UIMin = "1",UIMax = "4"))
	int32 CrowdAvoidanceQuality = 4;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crowd Avoidance Config", meta = (EditCondition = "bEnableCrowdAvoidance"))
	float CollisionQueryRange = 600.f;
};
