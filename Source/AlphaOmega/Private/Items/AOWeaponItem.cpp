// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/AOWeaponItem.h"
#include "Character/AOPlayer.h"
#include "Framework/AOPlayerController.h"


UAOWeaponItem::UAOWeaponItem()
{

}

bool UAOWeaponItem::Equip(class AAOPlayer* Character)
{
    bool bEquipSuccessful = Super::Equip(Character);
    if (bEquipSuccessful && Character)
    {
        Character->EquipWeapon(this);
    }
    return bEquipSuccessful;
}

bool UAOWeaponItem::Unequip(class AAOPlayer* Character)
{
    bool bUnequipSuccessful = Super::Unequip(Character);
    if (bUnequipSuccessful && Character)
    {
        Character->UnequipWeapon();
    }
    return bUnequipSuccessful;
}
