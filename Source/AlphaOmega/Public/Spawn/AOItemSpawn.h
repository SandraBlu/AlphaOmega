// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "Engine/DataTable.h"
#include "AOItemSpawn.generated.h"

USTRUCT(BlueprintType)
struct FLootTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:

	//Item(s) to Spawn
	UPROPERTY(EditDefaultsOnly, Category = "Loot")
	TArray<TSubclassOf<class UAOItem>> Items;

	//Percentage chance of spawning item based on Loot Roll
	UPROPERTY(EditDefaultsOnly, Category = "Loot", meta = (ClampMin = 0.001, ClampMax = 1.0))
	float Probability = 1.f;
	
};
/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API AAOItemSpawn : public ATargetPoint
{
	GENERATED_BODY()
	

		public:

	AAOItemSpawn();

	UPROPERTY(EditAnywhere, Category = "Loot")
	class UDataTable* LootTable;

	//Because pickups use BP Base, we use UProperty to select it
	UPROPERTY(EditDefaultsOnly, Category = "Loot")
	TSubclassOf<class AAOPickup> PickupClass;

	//Range used for generating respawn time
	UPROPERTY(EditDefaultsOnly, Category = "Loot")
	FIntPoint RespawnDelayRange;

protected:

	virtual void BeginPlay() override;
	
	FTimerHandle TimerHandle_RespawnItem;

	UPROPERTY()
	TArray<AActor*> SpawnedPickups;

	UFUNCTION()
	void SpawnItem();

	//keeps track of items destroyed to queue up next spawn
	UFUNCTION()
	void OnItemTaken(AActor* DestroyedActor);
};
