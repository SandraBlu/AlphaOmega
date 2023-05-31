// Copyright Epic Games, Inc. All Rights Reserved.


#include "AlphaOmegaGameModeBase.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "AI/AICharacter.h"
#include "Components/AOCoreComponent.h"
#include <EngineUtils.h>
#include <DrawDebugHelpers.h>
#include "Character/AOCharacter.h"
#include "Framework/AOPlayerController.h"
#include "AI/AOEnemyData.h"
#include <Engine/AssetManager.h>

AAlphaOmegaGameModeBase::AAlphaOmegaGameModeBase()
{
    SpawnTimerInterval = 2.f;

    CreditPerKill = 20;

    DesiredCreditCount = 10;
    RequiredCreditDistance = 2000;
}

void AAlphaOmegaGameModeBase::StartPlay()
{
    Super::StartPlay();
    //Spawn AI
    GetWorldTimerManager().SetTimer(TimerHandle_SpawnAI, this, &AAlphaOmegaGameModeBase::SpawnAITimerElapsed, SpawnTimerInterval, true);

    //Spawn Credit
//     if (ensure(CreditClasses.Num() > 0))
//     {
//         //Run EQS - made in Blueprint
//         UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, CreditSpawnQuery, this, EEnvQueryRunMode::AllMatching, nullptr);
//         if (IsValid(QueryInstance))
//         {
//             QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AAlphaOmegaGameModeBase::OnSpawnCreditQueryCompleted);
//         }
//     }
}

void AAlphaOmegaGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
    AAOCharacter* Player = Cast<AAOCharacter>(VictimActor);
    if (Player)
    {

        FTimerHandle TimerHandle_RespawnDelay;
        FTimerDelegate Delegate;
        Delegate.BindUFunction(this, "RespawnPlayerElapsed", Player->GetController());

        float RespawnDelay = 2.f;
        GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, 2.f, false);
    }
    //Give Credits For Kill
    AAOCharacter* KillerPawn = Cast<AAOCharacter>(Killer);
    if (KillerPawn && KillerPawn != VictimActor)
    {
        AAOPlayerController* PC = KillerPawn->GetController<AAOPlayerController>();
        if (PC)
        {
            PC->AddCredit(CreditPerKill);
        }

    }
    UE_LOG(LogTemp, Warning, TEXT("OnActor Killed: Victim: %s, killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(Killer));
}

void AAlphaOmegaGameModeBase::KillAll()
{
//     for (TActorIterator<AAICharacter> It(GetWorld()); It; ++It)
//     {
//         AAICharacter* AI = *It;
// 
//         UAOCoreComponent* AttributeComp;
//         if (IsValid(AttributeComp) && AttributeComp->IsAlive())
//         {
//             AttributeComp->Kill(this); // @fixme pass in player for kill credit
//         }
//     }
}

void AAlphaOmegaGameModeBase::SpawnAITimerElapsed()
{
    int32 NumOfAI = 0;
    for (TActorIterator<AAICharacter> It(GetWorld()); It; ++It)
    {
        AAICharacter* AI = *It;
        if (IsValid(AttributeComp) && AttributeComp->IsAlive())
        {
            NumOfAI++;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Found %i alive AI"), NumOfAI);

    float MaxAICount = 10.f;

    if (DifficultyCurve)
    {
        MaxAICount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
    }

    if (NumOfAI >= MaxAICount)
    {
        UE_LOG(LogTemp, Warning, TEXT("At Max AI Capacity. Skipping AI Spawn."));
        return;
    }

    UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnAIQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
    if (IsValid(QueryInstance))
    {
        QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AAlphaOmegaGameModeBase::OnQueryCompleted);
    }
}

//Load AI
void AAlphaOmegaGameModeBase::OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
    if (QueryStatus != EEnvQueryStatus::Success)
    {
        UE_LOG(LogTemp, Warning, TEXT("Spawn AI EQS Query failed!"));
        return;
    }

    TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

    if (Locations.IsValidIndex(0))
    {

        if (EnemyTable)
        {
            TArray<FEnemyInfo* > Rows;
            EnemyTable->GetAllRows("", Rows);

            //GetRandom Enemy
            int32 RandomIndex = FMath::RandRange(0, Rows.Num() - 1);
            FEnemyInfo* SelectedRow = Rows[RandomIndex];

            UAssetManager* Manager = UAssetManager::GetIfValid();
            if (Manager)
            {
                TArray<FName> Bundles;
                FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &AAlphaOmegaGameModeBase::OnEnemyLoaded, SelectedRow->EnemyID, Locations[0]);
                Manager->LoadPrimaryAsset(SelectedRow->EnemyID, Bundles, Delegate);
            }
        }
        //Track all used Spawn Locations
        DrawDebugSphere(GetWorld(), Locations[0], 50.f, 20, FColor::Blue, false, 60.f);
    }
}

//Spawn AI
void AAlphaOmegaGameModeBase::OnEnemyLoaded(FPrimaryAssetId LoadedID, FVector SpawnLocation)
{
    UAssetManager* Manager = UAssetManager::GetIfValid();
    if (Manager)
    {
        UAOEnemyData* EnemyData = Cast<UAOEnemyData>(Manager->GetPrimaryAssetObject(LoadedID));
        if (EnemyData)
        {
            AActor* NewEnemy = GetWorld()->SpawnActor<AActor>(EnemyData->EnemyClass, SpawnLocation, FRotator::ZeroRotator);
        }
    }
}

void AAlphaOmegaGameModeBase::Respawn(AController* Controller)
{
    if (IsValid(Controller))
    {
        Controller->UnPossess();

        RestartPlayer(Controller);
    }
}

void AAlphaOmegaGameModeBase::OnSpawnCreditQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
    if (QueryStatus != EEnvQueryStatus::Success)
    {
        UE_LOG(LogTemp, Warning, TEXT("Credit EQS Query Failed!"));
        return;
    }
    // Retrieve all possible locations that passed the query
    TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

    // Keep used locations to easily check distance between points
    TArray<FVector> UsedLocations;

    int32 SpawnCounter = 0;
    // Break out if we reached the desired count or if we have no more potential positions remaining
    while (SpawnCounter < DesiredCreditCount && Locations.Num() > 0)
    {
        // Pick a random location from remaining points.
        int32 RandomLocationIndex = FMath::RandRange(0, Locations.Num() - 1);

        FVector PickedLocation = Locations[RandomLocationIndex];
        // Remove to avoid picking again
        Locations.RemoveAt(RandomLocationIndex);

        // Check minimum distance requirement
        bool bValidLocation = true;
        for (FVector OtherLocation : UsedLocations)
        {
            float DistanceTo = (PickedLocation - OtherLocation).Size();

            if (DistanceTo < RequiredCreditDistance)
            {
                // Show skipped locations due to distance
                DrawDebugSphere(GetWorld(), PickedLocation, 50.0f, 20, FColor::Red, false, 10.0f);

                // too close, skip to next attempt
                bValidLocation = false;
                break;
            }
        }

        // Failed the distance test
        if (!bValidLocation)
        {
            continue;
        }

        // Pick a random credit class
        int32 RandomClassIndex = FMath::RandRange(0, CreditClasses.Num() - 1);
        TSubclassOf<AActor> RandomCreditClass = CreditClasses[RandomClassIndex];

        //Finally Spawn credit Actor at picked locations
        GetWorld()->SpawnActor<AActor>(RandomCreditClass, PickedLocation, FRotator::ZeroRotator);

        // Keep for distance checks
        UsedLocations.Add(PickedLocation);
        SpawnCounter++;
    }
}
