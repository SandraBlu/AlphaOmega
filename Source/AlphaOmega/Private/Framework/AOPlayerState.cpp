// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/AOPlayerState.h"
#include "AbilitySystem/AOAbilityComp.h"
#include "Attributes/AOAttributeSet.h"
#include "Net/UnrealNetwork.h"
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

void AAOPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAOPlayerState, Level);
	DOREPLIFETIME(AAOPlayerState, XP);
	DOREPLIFETIME(AAOPlayerState, AttributePts);
	DOREPLIFETIME(AAOPlayerState, AbilityPts);
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

void AAOPlayerState::SetAttributePoints(int32 InPoints)
{
	AttributePts = InPoints;
	OnAttributePtsChangeDelegate.Broadcast(AttributePts);
}

void AAOPlayerState::SetAbilityPoints(int32 InPoints)
{
	AbilityPts = InPoints;
	OnAbilityPtsChangeDelegate.Broadcast(AbilityPts);
}

void AAOPlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChangeDelegate.Broadcast(Level, true);
}

void AAOPlayerState::OnRep_XP(int32 OldXP)
{
	OnXPChangeDelegate.Broadcast(XP);
}

void AAOPlayerState::OnRep_AttributePts(int32 OldAttributePts)
{
	OnAttributePtsChangeDelegate.Broadcast(AttributePts);
}

void AAOPlayerState::OnRep_AbilityPts(int32 OldAbilityPts)
{
	OnAbilityPtsChangeDelegate.Broadcast(AbilityPts);
}

void AAOPlayerState::AddToLevel(int32 InLevel)
{
	Level += InLevel;
	OnLevelChangeDelegate.Broadcast(Level, true);
}

void AAOPlayerState::SetLevel(int32 InLevel)
{
	Level = InLevel;
	OnLevelChangeDelegate.Broadcast(Level, true);
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

