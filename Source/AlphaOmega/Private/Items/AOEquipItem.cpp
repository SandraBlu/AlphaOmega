// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/AOEquipItem.h"
#include "Character/AOPlayer.h"
#include "Components/AOInventoryComponent.h"

#define LOCTEXT_NAMESPACE "EquipItem"

UAOEquipItem::UAOEquipItem()
{
	bStackable = false;
	bEquipped = false;
	UseActionText = LOCTEXT("ItemUseActionText", "Equip");
}

void UAOEquipItem::Use(class AAOPlayer* Character)
{
	if (Character)
	{
		if (Character->GetEquippedItems().Contains(Slot) && !bEquipped)
		{
			UAOEquipItem* AlreadyEquippedItem = *Character->GetEquippedItems().Find(Slot);
			AlreadyEquippedItem->SetEquipped(false);
		}

		SetEquipped(!IsEquipped());
	}
}

bool UAOEquipItem::Equip(class AAOPlayer* Character)
{
	if (Character)
	{
		return Character->EquipItem(this);
	}
	return false;
}

bool UAOEquipItem::Unequip(class AAOPlayer* Character)
{

	if (Character)
	{
		return Character->UnequipItem(this);
	}
	return false;
}

bool UAOEquipItem::ShouldShowInInventory() const
{
	return !bEquipped;
}

void UAOEquipItem::AddedToInventory(class UAOInventoryComponent* Inventory)
{
	// if item already equipped, don't auto equip
	if (AAOPlayer* Character = Cast<AAOPlayer>(Inventory->GetOwner()))
	{
		if (Character && !Character->IsLooting())
		{
			// if item not equipped, auto equip
			if (!Character->GetEquippedItems().Contains(Slot))
			{
				SetEquipped(true);
			}
		}
	}
}

void UAOEquipItem::SetEquipped(bool bNewEquipped)
{
	bEquipped = bNewEquipped;
	EquipStatusChanged();
}

void UAOEquipItem::EquipStatusChanged()
{
	if (AAOPlayer* Character = Cast<AAOPlayer>(GetOuter()))
	{
		if (bEquipped)
		{
			Equip(Character);
		}
		else
		{
			Unequip(Character);
		}
	}
	//Update UI
	OnItemModified.Broadcast();
}

#undef  LOCTEXT_NAMESPACE