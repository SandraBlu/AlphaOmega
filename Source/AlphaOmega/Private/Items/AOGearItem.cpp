// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/AOGearItem.h"
#include "Character/AOPlayer.h"

UAOGearItem::UAOGearItem()
{
	DamageDefenseMult = .1f;
}

bool UAOGearItem::Equip(class AAOPlayer* Character)
{
	bool bEquipSuccessful = Super::Equip(Character);

	if (bEquipSuccessful && Character)
	{
		Character->EquipGear(this);
	}
	return bEquipSuccessful;
}

bool UAOGearItem::Unequip(class AAOPlayer* Character)
{
	bool bUnEquipSuccessful = Super::Unequip(Character);

	if (bUnEquipSuccessful && Character)
	{
		Character->UnequipGear(Slot);
	}
	return bUnEquipSuccessful;
}
