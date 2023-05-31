// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "Engine/DataTable.h"
#include "AlphaOmegaGameModeBase.generated.h"

class UCurveFloat;
class UDataTable;
class UAOEnemyData;
class UAOSaveGame;
class UAOCoreComponent;

/* Data Table Row for Spawning Enemies in Game Mode */
USTRUCT(BlueprintType)
struct FEnemyInfo : public FTableRowBase
{
	GENERATED_BODY()

public:

	FEnemyInfo()
	{
		Weight = 1.0f;
		SpawnCost = 5.0f;
		KillReward = 20.0f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPrimaryAssetId EnemyID;
	
		
		
	/* Relative chance to pick this monster */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight;

	/* Points required by gamemode to spawn this unit. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnCost;

	/* Amount of credits awarded to killer of this unit.  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillReward;

};

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API AAlphaOmegaGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AAlphaOmegaGameModeBase();
	UAOCoreComponent* AttributeComp;

	virtual void StartPlay() override;


	virtual void OnActorKilled(AActor* VictimActor, AActor* Killer);

	//Built In Cheat For Testing Only
	UFUNCTION(Exec)
	void KillAll();

	protected:


	// Read/write access as we could change this as our difficulty increases via Blueprint
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	int32 CreditPerKill;
	

	//AI Spawning

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UDataTable* EnemyTable;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<AActor> EnemyClass;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* SpawnAIQuery;

	FTimerHandle TimerHandle_SpawnAI;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SpawnTimerInterval;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* DifficultyCurve;

	//Credit Spawning
	UPROPERTY(EditDefaultsOnly, Category = "Credit")
	TObjectPtr<UEnvQuery> CreditSpawnQuery;
	/* All Credit classes used to spawn with EQS at match start */
	UPROPERTY(EditDefaultsOnly, Category = "Credit")
	TArray<TSubclassOf<AActor>> CreditClasses;

	/* Distance required between credit spawn locations */
	UPROPERTY(EditDefaultsOnly, Category = "Credit")
	float RequiredCreditDistance;

	/* Amount of credits to spawn during match start */
	UPROPERTY(EditDefaultsOnly, Category = "Credit")
	int32 DesiredCreditCount;

	UFUNCTION()
	void SpawnAITimerElapsed();

	UFUNCTION()
	void OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	void OnEnemyLoaded(FPrimaryAssetId LoadedID, FVector SpawnLocation);

	UFUNCTION()
	void Respawn(AController* Controller);

	UFUNCTION()
	void OnSpawnCreditQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

};
