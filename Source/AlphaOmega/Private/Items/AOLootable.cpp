// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/AOLootable.h"
#include "Components/InteractComponent.h"
#include "Components/AOInventoryComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/AOPlayer.h"
#include "Spawn/AOItemSpawn.h"
#include "Items/AOItem.h"

#define LOCTEXT_NAMESPACE "Loot"

// Sets default values
AAOLootable::AAOLootable()
{
	LootContainerMesh = CreateDefaultSubobject<UStaticMeshComponent>("LootContainerMesh");
	SetRootComponent(LootContainerMesh);

	LootInteract = CreateDefaultSubobject<UInteractComponent>("LootInteract");
	LootInteract->InteractActionText = LOCTEXT("LootActorText", "Loot");
	LootInteract->InteractItemName = LOCTEXT("LootActorName", "Cache");
	LootInteract->SetupAttachment(GetRootComponent());

	Inventory = CreateDefaultSubobject<UAOInventoryComponent>("Inventory");
	Inventory->SetCapacity(20);
	Inventory->SetWeightCapacity(80.f);

	LootRolls = FIntPoint(2, 8);
}

// Called when the game starts or when spawned
void AAOLootable::BeginPlay()
{
	Super::BeginPlay();

	LootInteract->OnInteract.AddDynamic(this, &AAOLootable::OnInteract);

 	if (LootTable)
 	{
 		TArray<FLootTableRow*> SpawnItems;
 		LootTable->GetAllRows("", SpawnItems);
 
 		int32 Rolls = FMath::RandRange(LootRolls.GetMin(), LootRolls.GetMax());
 
 		for (int32 i = 0; i < Rolls; ++i)
 		{
 			const FLootTableRow* LootRow = SpawnItems[FMath::RandRange(0, SpawnItems.Num() - 1)];
 
 			ensure(LootRow);
 
 			float ProbabilityRoll = FMath::FRandRange(0.f, 1.f);
 
 			while (ProbabilityRoll > LootRow->Probability)
 			{
 				LootRow = SpawnItems[FMath::RandRange(0, SpawnItems.Num() - 1)];
 				ProbabilityRoll = FMath::FRandRange(0.f, 1.f);
 			}
 			if (LootRow && LootRow->Items.Num())
 			{
 				for (auto& ItemClass : LootRow->Items)
 				{
 					if (ItemClass)
 					{
 						const int32 Quantity = Cast<UAOItem>(ItemClass->GetDefaultObject())->GetQuantity();
 						Inventory->TryAddItemFromClass(ItemClass, Quantity);
 					}
 				}
 			}
 		}
 	}
}

void AAOLootable::OnInteract(class AAOPlayer* Character)
{
	if (Character)
	{
		Character->SetLootSource(Inventory);
	}
}


#undef LOCTEXT_NAMESPACE