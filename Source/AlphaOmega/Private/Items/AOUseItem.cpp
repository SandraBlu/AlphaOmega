// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/AOUseItem.h"
#include "Characters/AOPlayer.h"
#include "Framework/AOPlayerController.h"
#include "Components/AOInventoryComponent.h"


#define LOCTEXT_NAMESPACE "Use Item"

UAOUseItem::UAOUseItem()
{
	UseActionText = LOCTEXT("ItemUseActionText", "Consume");
}

void UAOUseItem::Use(class AAOPlayer* Character)
{
	if (Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("Used Item"));
		if (AAOPlayerController* PC = Cast<AAOPlayerController>(Character->GetController()))
		{
			PC->ShowNotification(FText::Format(LOCTEXT("UsedItemText", "Used {ItemName}, restored {RestoreAmount}."), ItemDisplayName, RestoreAmount));
		}
		else
		{
			PC->ShowNotification(FText::Format(LOCTEXT("FullAttributeText", "No need to use {ItemName}. Already FULL. "), ItemDisplayName, RestoreAmount));
		}
	}
	if (UAOInventoryComponent* Inventory = Character->PlayerInventory)
	{
		Inventory->ConsumeItem(this, 1);
	}
}
