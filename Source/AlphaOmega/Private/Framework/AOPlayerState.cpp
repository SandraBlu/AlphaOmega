// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/AOPlayerState.h"
#include "AbilitySystem/AOAbilityComp.h"
#include "Attributes/AOAttributeSet.h"
#include "Components/AOInventoryComponent.h"

AAOPlayerState::AAOPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAOAbilityComp>("ASC");
	AttributeSet = CreateDefaultSubobject<UAOAttributeSet>("Attributes");

	PlayerInventory = CreateDefaultSubobject<UAOInventoryComponent>("InventoryComp");
	PlayerInventory->SetCapacity(25);
	PlayerInventory->SetWeightCapacity(60.f);
	
	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* AAOPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAOPlayerState::AddToXP(int32 InXP)
{
	XP += InXP;
	OnXPChangeDelegate.Broadcast(XP);
}

void AAOPlayerState::SetXP(int32 InXP)
{
	XP = InXP;
	OnXPChangeDelegate.Broadcast(XP);
}

void AAOPlayerState::AddToLevel(int32 InLevel)
{
	Level += InLevel;
	OnLevelChangeDelegate.Broadcast(Level);
}

void AAOPlayerState::SetLevel(int32 InLevel)
{
	Level = InLevel;
	OnLevelChangeDelegate.Broadcast(Level);
}

void AAOPlayerState::AddToAttributePts(int32 InAttributePts)
{
	AttributePts += InAttributePts;
	OnAttributePtsChangeDelegate.Broadcast(AttributePts);
}

void AAOPlayerState::AddToAbilityPts(int32 InAbilityPts)
{
	AbilityPts += InAbilityPts;
	OnAbilityPtsChangeDelegate.Broadcast(AbilityPts);
}

