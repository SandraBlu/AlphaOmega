// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/AOInventoryBFL.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Components/AOInventoryComponent.h"
#include "Item/AOItem.h"

class UAOInventoryComponent* UAOInventoryBFL::GetInventoryComponentFromTarget(AActor* Target)
{
	if (!Target)
	{
		return nullptr;
	}

	if (UAOInventoryComponent* InventoryComp = Target->FindComponentByClass<UAOInventoryComponent>())
	{
		return InventoryComp;
	}

	//Try player state, then pawn, then controller 
	if (APawn* OwningPawn = Cast<APawn>(Target))
	{
		if (OwningPawn->GetPlayerState())
		{
			return OwningPawn->GetPlayerState()->FindComponentByClass<UAOInventoryComponent>();
		}

		if (OwningPawn->GetController())
		{
			return OwningPawn->GetController()->FindComponentByClass<UAOInventoryComponent>();
		}
	}

	//Same for controller based target 
	if (APlayerController* OwningController = Cast<APlayerController>(Target))
	{
		if (OwningController->GetPawn())
		{
			if (OwningController->GetPlayerState<APlayerState>())
			{
				return OwningController->GetPlayerState<APlayerState>()->FindComponentByClass<UAOInventoryComponent>();
			}


			return OwningController->GetPawn()->FindComponentByClass<UAOInventoryComponent>();
		}
	}

	return nullptr;
}

TArray<class UAOItem*> UAOInventoryBFL::SortItemArrayAlphabetical(TArray<class UAOItem*> InItems, const bool bReverse)
{
	TArray<class UAOItem*> RetItems = InItems;

	//Sort the replies by their Y position in the graph
	if (bReverse)
	{
		RetItems.Sort([](const UAOItem& ItemA, const UAOItem& ItemB) {
			return ItemA.DisplayName.ToString() < ItemB.DisplayName.ToString();
			});
	}
	else
	{
		RetItems.Sort([](const UAOItem& ItemA, const UAOItem& ItemB) {
			return ItemA.DisplayName.ToString() > ItemB.DisplayName.ToString();
			});
	}

	return RetItems;
}

TArray<class UAOItem*> UAOInventoryBFL::SortItemArrayWeight(TArray<class UAOItem*> InItems, const bool bReverse)
{
	TArray<class UAOItem*> RetItems = InItems;

	//Sort the replies by their Y position in the graph
	if (bReverse)
	{
		RetItems.Sort([](const UAOItem& ItemA, const UAOItem& ItemB) {
			return ItemA.GetStackWeight() > ItemB.GetStackWeight();
			});
	}
	else
	{
		RetItems.Sort([](const UAOItem& ItemA, const UAOItem& ItemB) {
			return ItemA.GetStackWeight() < ItemB.GetStackWeight();
			});
	}


	return RetItems;
}