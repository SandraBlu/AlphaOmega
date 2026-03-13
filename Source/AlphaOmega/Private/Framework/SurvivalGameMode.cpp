// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/SurvivalGameMode.h"

#include "NavigationSystem.h"
#include "AI/NPCBase.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "DebugHelper.h"

ASurvivalGameMode::ASurvivalGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ASurvivalGameMode::RegisterSpawnedEnemies(const TArray<ANPCBase*>& InEnemiesToRegister)
{
	for (ANPCBase* SpawnedEnemy : InEnemiesToRegister)
	{
		if (SpawnedEnemy)
		{
			CurrentSpawnedEnemiesCounter++;
			SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this,&ThisClass::OnEnemyDestroyed);
		}
	}
}

void ASurvivalGameMode::BeginPlay()
{
	Super::BeginPlay();
	checkf(EnemyWaveSpawnerDataTable,TEXT("Forgot to assign a valid data table in survival game mode blueprint"));
	SetCurrentSurvivalState(ESurvivalState::WaitSpawnNewWave);
	TotalWavesToSpawn = EnemyWaveSpawnerDataTable->GetRowNames().Num();
	PreLoadNextWaveEnemies();
}

void ASurvivalGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentSurvivalState == ESurvivalState::WaitSpawnNewWave)
	{
		TimePassedSinceStart += DeltaTime;
		if (TimePassedSinceStart >= SpawnNewWaveWaitTime)
		{
			TimePassedSinceStart = 0.f;
			SetCurrentSurvivalState(ESurvivalState::SpawningNewWave);
		}
	}
	if (CurrentSurvivalState == ESurvivalState::SpawningNewWave)
	{
		TimePassedSinceStart += DeltaTime;
		if (TimePassedSinceStart >= SpawnEnemiesDelayTime)
		{
			CurrentSpawnedEnemiesCounter += TrySpawnWaveEnemies();
			TimePassedSinceStart = 0.f;
			SetCurrentSurvivalState(ESurvivalState::InProgress);
		}
	}
	if (CurrentSurvivalState == ESurvivalState::WaveCompleted)
	{
		TimePassedSinceStart += DeltaTime;
		if (TimePassedSinceStart >= WaveCompletedWaitTime)
		{
			TimePassedSinceStart = 0.f;
			CurrentWaveCount++;
			if (HasFinishedAllWaves())
			{
				SetCurrentSurvivalState(ESurvivalState::AllWavesDone);
			}
			else
			{
				SetCurrentSurvivalState(ESurvivalState::WaitSpawnNewWave);
				PreLoadNextWaveEnemies();
			}
		}
	}
}

void ASurvivalGameMode::SetCurrentSurvivalState(ESurvivalState InState)
{
	CurrentSurvivalState = InState;
	OnSurvivalStateChanged.Broadcast(CurrentSurvivalState);
}
bool ASurvivalGameMode::HasFinishedAllWaves() const
{
	return CurrentWaveCount>TotalWavesToSpawn;
}

void ASurvivalGameMode::PreLoadNextWaveEnemies()
{
	if (HasFinishedAllWaves())
	{
		return;
	}
	PreLoadedEnemyClassMap.Empty();
	
	for (const FEnemyWaveSpawnerInfo& SpawnerInfo : GetCurrentWaveSpawnerTableRow()->WaveSpawnerDefinitions)
	{
		if(SpawnerInfo.SoftEnemyClassToSpawn.IsNull()) continue;
		UAssetManager::GetStreamableManager().RequestAsyncLoad(SpawnerInfo.SoftEnemyClassToSpawn.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda([SpawnerInfo,this]()
				{
					if (UClass* LoadedEnemyClass = SpawnerInfo.SoftEnemyClassToSpawn.Get())
					{
						PreLoadedEnemyClassMap.Emplace(SpawnerInfo.SoftEnemyClassToSpawn,LoadedEnemyClass);
					}
				}
			)
		);
	}
}

FEnemyWaveSpawnerTableRow* ASurvivalGameMode::GetCurrentWaveSpawnerTableRow() const
{
	const FName RowName = FName(TEXT("Wave") + FString::FromInt(CurrentWaveCount));
	FEnemyWaveSpawnerTableRow* FoundRow = EnemyWaveSpawnerDataTable->FindRow<FEnemyWaveSpawnerTableRow>(RowName,FString());
	
	checkf(FoundRow,TEXT("Could not find a valid row under the name %s in the data table"),*RowName.ToString());
	return FoundRow;
}

int32 ASurvivalGameMode::TrySpawnWaveEnemies()
{
	if (TargetPointsArray.IsEmpty())
	{
		UGameplayStatics::GetAllActorsOfClass(this,ATargetPoint::StaticClass(),TargetPointsArray);
	}
	
	checkf(!TargetPointsArray.IsEmpty(),TEXT("No valid target point found in level: %s for spawning enemies"),*GetWorld()->GetName());
	uint32 EnemiesSpawnedThisTime = 0;
	FActorSpawnParameters SpawnParam;
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	for (const FEnemyWaveSpawnerInfo& SpawnerInfo : GetCurrentWaveSpawnerTableRow()->WaveSpawnerDefinitions)
	{
		if(SpawnerInfo.SoftEnemyClassToSpawn.IsNull()) continue;
		const int32 NumToSpawn = FMath::RandRange(SpawnerInfo.MinPerSpawnCount,SpawnerInfo.MaxPerSpawnCount);
		UClass* LoadedEnemyClass = PreLoadedEnemyClassMap.FindChecked(SpawnerInfo.SoftEnemyClassToSpawn);
		for (int32 i = 0; i < NumToSpawn; i++)
		{
			const int32 RandomTargetPointIndex = FMath::RandRange(0,TargetPointsArray.Num() - 1);
			const FVector SpawnOrigin = TargetPointsArray[RandomTargetPointIndex]->GetActorLocation();
			const FRotator SpawnRotation = TargetPointsArray[RandomTargetPointIndex]->GetActorForwardVector().ToOrientationRotator();
			FVector RandomLocation;
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this,SpawnOrigin,RandomLocation,400.f);
			RandomLocation += FVector(0.f,0.f,150.f);
			ANPCBase* SpawnedEnemy = GetWorld()->SpawnActor<ANPCBase>(LoadedEnemyClass,RandomLocation,SpawnRotation,SpawnParam);
			
			if (SpawnedEnemy)
			{
				SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this,&ThisClass::OnEnemyDestroyed);
				EnemiesSpawnedThisTime++;
				TotalSpawnedEnemiesThisWaveCounter++;
			}
			
			if (!ShouldKeepSpawnEnemies())
			{
				return EnemiesSpawnedThisTime;
			}
		}
	}
	return EnemiesSpawnedThisTime;
}

bool ASurvivalGameMode::ShouldKeepSpawnEnemies() const
{
	return TotalSpawnedEnemiesThisWaveCounter < GetCurrentWaveSpawnerTableRow()->TotalEnemyToSpawnThisWave;
}

void ASurvivalGameMode::OnEnemyDestroyed(AActor* DestroyedActor)
{
	CurrentSpawnedEnemiesCounter--;
	Debug::Print(FString::Printf(TEXT("CurrentSpawnedEnemiesCounter:%i, TotalSpawnedEnemiesThisWaveCounter:%i"),CurrentSpawnedEnemiesCounter,TotalSpawnedEnemiesThisWaveCounter));

	if (ShouldKeepSpawnEnemies())
	{
		CurrentSpawnedEnemiesCounter += TrySpawnWaveEnemies();
	}
	else if (CurrentSpawnedEnemiesCounter == 0)
	{
		TotalSpawnedEnemiesThisWaveCounter = 0;
		CurrentSpawnedEnemiesCounter = 0;
		SetCurrentSurvivalState(ESurvivalState::WaveCompleted);
	}
}