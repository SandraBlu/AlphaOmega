// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/AOItem.h"
#include "Components/AOInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "UObject/UnrealType.h"
#include "Engine/BlueprintGeneratedClass.h"

#define LOCTEXT_NAMESPACE "Item"

UAOItem::UAOItem()
{
	DisplayName = LOCTEXT("ItemName", "Item");
	UseActionText = LOCTEXT("UseActionText", "Use");
	Weight = 0.f;
	bStackable = true;
	Quantity = 1;
	MaxStackSize = 2;
	RepKey = 0;
	LastUseTime = -FLT_MAX;
	BaseValue = 10;

	FString NameString = GetName();

	//Add the Default Speaker to the Quest 
	int32 UnderscoreIndex = -1;

	if (NameString.FindChar(TCHAR('_'), UnderscoreIndex))
	{
		//remove item name prefix
		DisplayName = FText::FromString(FName::NameToDisplayString(NameString.RightChop(UnderscoreIndex + 1), false));
	}
	else
	{
		DisplayName = FText::FromString(NameString);
	}

	Stats.Add("Weight");
	Stats.Add("Quantity");
}

void UAOItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAOItem, Quantity);
	DOREPLIFETIME_CONDITION_NOTIFY(UAOItem, bActive, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION(UAOItem, LastUseTime, COND_OwnerOnly);
}

bool UAOItem::IsSupportedForNetworking() const
{
	return true;
}

UWorld* UAOItem::GetWorld() const
{
	return World;
}

#if WITH_EDITOR
void UAOItem::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	//UPROPERTY clamping doesn't support using a variable to clamp so we do in here instead
	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UAOItem, Quantity))
	{
		Quantity = FMath::Clamp(Quantity, 1, GetMaxStackSize());
	}
	else if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UAOItem, bStackable))
	{
		if (!bStackable)
		{
			Quantity = 1;
		}
	}
}

bool UAOItem::HasAuthority() const
{
	return (OwningInventory && OwningInventory->GetOwnerRole() >= ROLE_Authority);
}

#endif

void UAOItem::OnRep_Quantity(const int32 OldQuantity)
{
	OnItemModified.Broadcast();
}

void UAOItem::OnRep_bActive(const bool bOldActive)
{
	if (bActive)
	{
		Activated();
	}
	else
	{
		Deactivated();
	}
}

void UAOItem::SetActive(const bool bNewActive)
{
	if (CanActivate() && bNewActive != bActive)
	{
		bActive = bNewActive;
		OnRep_bActive(!bActive);
		MarkDirtyForReplication();
	}
}

void UAOItem::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity != Quantity)
	{
		const int32 OldQuantity = Quantity;
		Quantity = FMath::Clamp(NewQuantity, 0, GetMaxStackSize());
		OnRep_Quantity(OldQuantity);
		MarkDirtyForReplication();
	}
}

void UAOItem::SetLastUseTime(const float NewLastUseTime)
{
	LastUseTime = NewLastUseTime;
	MarkDirtyForReplication();
}

void UAOItem::Use()
{
}

void UAOItem::AddedToInventory(UAOInventoryComponent* Inventory)
{
	
}

void UAOItem::RemovedFromInventory(UAOInventoryComponent* Inventory)
{
	if (bActive)
	{
		Deactivated();
	}
}

void UAOItem::MarkDirtyForReplication()
{
	//Mark this object for replication
	++RepKey;

	//Mark the array for replication
	if (OwningInventory)
	{
		++OwningInventory->ReplicatedItemsKey;
	}
}

bool UAOItem::ShouldShowInInventory_Implementation() const
{
	return true;
}

bool UAOItem::CanBeRemoved_Implementation() const
{
	return true;
}

void UAOItem::Activated_Implementation()
{
}

void UAOItem::Deactivated_Implementation()
{
}

bool UAOItem::CanActivate_Implementation() const
{
	return bCanActivate;
}

bool UAOItem::CanUse_Implementation() const
{
	return true;
}

FText UAOItem::GetParsedDescription()
{
	//Replace variables in dialogue line
    FString LineString = Description.ToString();
    
    int32 OpenBraceIdx = -1;
    int32 CloseBraceIdx = -1;
    bool bFoundOpenBrace = LineString.FindChar('{', OpenBraceIdx);
    bool bFoundCloseBrace = LineString.FindChar('}', CloseBraceIdx);
    uint32 Iters = 0; // More than 50 wildcard replaces and something has probably gone wrong, so safeguard against that
    
    while (bFoundOpenBrace && bFoundCloseBrace && OpenBraceIdx < CloseBraceIdx && Iters < 50)
    {
    	const FString VariableName = LineString.Mid(OpenBraceIdx + 1, CloseBraceIdx - OpenBraceIdx - 1);
    	const FString VariableVal = GetStringVariable(VariableName);
    
    	if (!VariableVal.IsEmpty())
    	{
    		LineString.RemoveAt(OpenBraceIdx, CloseBraceIdx - OpenBraceIdx + 1);
    		LineString.InsertAt(OpenBraceIdx, VariableVal);
    	}
    
    	bFoundOpenBrace = LineString.FindChar('{', OpenBraceIdx);
    	bFoundCloseBrace = LineString.FindChar('}', CloseBraceIdx);
    
    	Iters++;
    }
    
    return FText::FromString(LineString);
}

FString UAOItem::GetStringVariable_Implementation(const FString& VariableName)
{
	//Overriable in BP in case you want to add more 
	if (VariableName == "Display Name")
	{
		return DisplayName.ToString();
	}
	else if (VariableName == "Weight")
	{
		return FString::SanitizeFloat(Weight);
	}
	else if (VariableName == "Recharge Duration")
	{
		return FString::SanitizeFloat(UseRechargeDuration);
	}
	else if (VariableName == "Stack Weight")
	{
		return FString::SanitizeFloat(GetStackWeight());
	}
	else if (VariableName == "Quantity")
	{
		return FString::FromInt(Quantity);
	}
	else if (VariableName == "Max Stack Size")
	{
		return FString::FromInt(MaxStackSize);
	}
	else if (VariableName == "BaseValue")
	{
		return FString::FromInt(BaseValue);
	}

	return FString();
}

APlayerController* UAOItem::GetOwningController() const
{
	if (OwningInventory)
	{
		return OwningInventory->GetOwningController();
	}

	return nullptr;
}

APawn* UAOItem::GetOwningPawn() const
{
	if (OwningInventory)
	{
		return OwningInventory->GetOwningPawn();
	}
	return nullptr;
}

#undef LOCTEXT_NAMESPACE