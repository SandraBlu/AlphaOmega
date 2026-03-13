// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryBFL.h"

#include "GameFramework/PlayerState.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/Item.h"

class UInventoryComponent* UInventoryBFL::GetInventoryComponentFromTarget(AActor* Target)
{
	if (!Target)
	{
		return nullptr;
	}

	if (UInventoryComponent* InventoryComp = Target->FindComponentByClass<UInventoryComponent>())
	{
		return InventoryComp;
	}

	//Try player state, then pawn, then controller 
	if (APawn* OwningPawn = Cast<APawn>(Target))
	{
		if (OwningPawn->GetPlayerState())
		{
			return OwningPawn->GetPlayerState()->FindComponentByClass<UInventoryComponent>();
		}

		if (OwningPawn->GetController())
		{
			return OwningPawn->GetController()->FindComponentByClass<UInventoryComponent>();
		}
	}

	//Same for controller based target 
	if (APlayerController* OwningController = Cast<APlayerController>(Target))
	{
		if (OwningController->GetPawn())
		{
			if (OwningController->GetPlayerState<APlayerState>())
			{
				return OwningController->GetPlayerState<APlayerState>()->FindComponentByClass<UInventoryComponent>();
			}


			return OwningController->GetPawn()->FindComponentByClass<UInventoryComponent>();
		}
	}

	return nullptr;
}

TArray<class UItem*> UInventoryBFL::SortItemArrayAlphabetical(TArray<class UItem*> InItems, const bool bReverse)
{
	TArray<class UItem*> RetItems = InItems;

	//Sort the replies by their Y position in the graph
	if (bReverse)
	{
		RetItems.Sort([](const UItem& ItemA, const UItem& ItemB) {
			return ItemA.DisplayName.ToString() < ItemB.DisplayName.ToString();
			});
	}
	else
	{
		RetItems.Sort([](const UItem& ItemA, const UItem& ItemB) {
			return ItemA.DisplayName.ToString() > ItemB.DisplayName.ToString();
			});
	}

	return RetItems;
}

TArray<class UItem*> UInventoryBFL::SortItemArrayWeight(TArray<class UItem*> InItems, const bool bReverse)
{
	TArray<class UItem*> RetItems = InItems;

	//Sort the replies by their Y position in the graph
	if (bReverse)
	{
		RetItems.Sort([](const UItem& ItemA, const UItem& ItemB) {
			return ItemA.GetStackWeight() > ItemB.GetStackWeight();
			});
	}
	else
	{
		RetItems.Sort([](const UItem& ItemA, const UItem& ItemB) {
			return ItemA.GetStackWeight() < ItemB.GetStackWeight();
			});
	}


	return RetItems;
}
