// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/OPlayerState.h"

#include "GAS/OAbilitySystemComponent.h"
#include "GAS/OAttributeSet.h"
#include "Inventory/InventoryComponent.h"
#include "Net/UnrealNetwork.h"

AOPlayerState::AOPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UOAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<UOAttributeSet>("AttributeSet");

	PlayerInventory = CreateDefaultSubobject<UInventoryComponent>("InventoryComp");
	PlayerInventory->SetCapacity(25);
	PlayerInventory->SetWeightCapacity(60.f);
	
	NetUpdateFrequency = 100.f;
}

void AOPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AOPlayerState, Level);
	DOREPLIFETIME(AOPlayerState, XP);
	DOREPLIFETIME(AOPlayerState, AttributePoints);
	DOREPLIFETIME(AOPlayerState, AbilityPoints);
}

UAbilitySystemComponent* AOPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AOPlayerState::AddToXP(int32 InXP)
{
	XP += InXP;
	OnXPChangeDelegate.Broadcast(XP);
}

void AOPlayerState::AddToLevel(int32 InLevel)
{
	Level += InLevel;
	OnLevelChangeDelegate.Broadcast(Level);
}

void AOPlayerState::AddToAttributePts(int32 InAttributePts)
{
	AttributePoints += InAttributePts;
	OnAttributePtsChangeDelegate.Broadcast(AttributePoints);
}

void AOPlayerState::AddToAbilityPts(int32 InAbilityPts)
{
	AbilityPoints += InAbilityPts;
	OnAbilityPtsChangeDelegate.Broadcast(AbilityPoints);
}

void AOPlayerState::SetXP(int32 InXP)
{
	XP = InXP;
	OnXPChangeDelegate.Broadcast(XP);
}

void AOPlayerState::SetLevel(int32 InLevel)
{
	Level = InLevel;
	OnLevelChangeDelegate.Broadcast(Level);
}

void AOPlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChangeDelegate.Broadcast(Level);
}

void AOPlayerState::OnRep_XP(int32 OldXP)
{
	OnXPChangeDelegate.Broadcast(XP);
}

void AOPlayerState::OnRep_AttributePoints(int32 OldAttributePoints)
{
	OnAttributePtsChangeDelegate.Broadcast(AttributePoints);
}

void AOPlayerState::OnRep_AbilityPoints(int32 OldAbilityPoints)
{
	OnAbilityPtsChangeDelegate.Broadcast(AbilityPoints);
}