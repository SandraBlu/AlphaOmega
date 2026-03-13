// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Item.h"

#include "Inventory/InventoryComponent.h"
#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "Item"

#define ItemStat_DisplayName "DisplayName"
#define ItemStat_Weight "Weight"
#define ItemStat_Quantity "Quantity"
#define ItemStat_RechargeDuration "RechargeDuration"
#define ItemStat_StackWeight "StackWeight"
#define ItemStat_MaxStackSize "MaxStackSize"
#define ItemStat_BaseValue "BaseValue"

UItem::UItem()
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

	Stats.Add(FItemStat(LOCTEXT("WeightStatDisplayText", "Weight"), ItemStat_Weight));
	Stats.Add(FItemStat(LOCTEXT("QuantityStatDisplayText", "Quantity"), ItemStat_Quantity));
}

void UItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	//Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//Inventory needs items to support user added blueprint replicated variables 
	UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass());
	if (BPClass != NULL)
	{
		BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
	}

	DOREPLIFETIME(UItem, Quantity);
	DOREPLIFETIME_CONDITION_NOTIFY(UItem, bActive, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION(UItem, LastUseTime, COND_OwnerOnly);
}

bool UItem::IsSupportedForNetworking() const
{
	return true;
}

UWorld* UItem::GetWorld() const
{
	return World;
}

#if WITH_EDITOR
void UItem::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	//UPROPERTY clamping doesn't support using a variable to clamp so we do in here instead
	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UItem, Quantity))
	{
		Quantity = FMath::Clamp(Quantity, 1, GetMaxStackSize());
	}
	else if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UItem, bStackable))
	{
		if (!bStackable)
		{
			Quantity = 1;
		}
	}
}
#endif

bool UItem::HasAuthority() const
{
	return (OwningInventory && OwningInventory->GetOwnerRole() >= ROLE_Authority);
}

void UItem::OnRep_bActive(const bool bOldActive)
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

void UItem::SetActive(const bool bNewActive, const bool bForce)
{
	if (bCanActivate)
	{
		if (bNewActive != bActive || bForce)
		{
			bActive = bNewActive;
			OnRep_bActive(!bActive);
			MarkDirtyForReplication();
		}
	}
}

void UItem::OnRep_Quantity(const int32 OldQuantity)
{
	OnItemModified.Broadcast();
}

void UItem::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity != Quantity)
	{
		const int32 OldQuantity = Quantity;
		Quantity = FMath::Clamp(NewQuantity, 0, GetMaxStackSize());
		OnRep_Quantity(OldQuantity);
		MarkDirtyForReplication();
	}
}

void UItem::SetLastUseTime(const float NewLastUseTime)
{
	LastUseTime = NewLastUseTime;
	MarkDirtyForReplication();
}

void UItem::Use()
{
}

void UItem::AddedToInventory(UInventoryComponent* Inventory)
{

}

void UItem::RemovedFromInventory(UInventoryComponent* Inventory)
{
	if (bActive)
	{
		Deactivated();
	}
}

void UItem::MarkDirtyForReplication()
{
	//Mark this object for replication
	++RepKey;

	//Mark the array for replication
	if (OwningInventory)
	{
		++OwningInventory->ReplicatedItemsKey;
	}
}

bool UItem::ShouldShowInInventory_Implementation() const
{
	//By default, don't show vendors equipped items in their store  
	if (OwningInventory && OwningInventory->bIsVendor)
	{
		if (bActive)
		{
			return false; 
		}
	}

	return true;
}

bool UItem::CanBeRemoved_Implementation() const
{
	return true;
}

void UItem::Activated_Implementation()
{
}

void UItem::Deactivated_Implementation()
{
}

bool UItem::CanUse_Implementation() const
{
	return true;
}

bool UItem::ShouldUseOnAdd_Implementation() const
{
	return false;
}

FText UItem::GetRawDescription_Implementation()
{
	return Description;
}

FText UItem::GetParsedDescription()
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

FString UItem::GetStringVariable_Implementation(const FString& VariableName)
{
	//Overriable in BP in case you want to add more 
	if (VariableName == ItemStat_DisplayName)
	{
		return DisplayName.ToString();
	}
	else if (VariableName == ItemStat_Weight)
	{
		return FString::SanitizeFloat(Weight);
	}
	else if (VariableName == ItemStat_RechargeDuration)
	{
		return FString::SanitizeFloat(UseRechargeDuration);
	}
	else if (VariableName == ItemStat_StackWeight)
	{
		return FString::SanitizeFloat(GetStackWeight());
	}
	else if (VariableName == ItemStat_Quantity)
	{
		return FString::FromInt(Quantity);
	}
	else if (VariableName == ItemStat_MaxStackSize)
	{
		return FString::FromInt(MaxStackSize);
	}
	else if (VariableName == ItemStat_BaseValue)
	{
		return FString::FromInt(BaseValue);
	}

	return FString();
}

APlayerController* UItem::GetOwningController() const
{
	if (OwningInventory)
	{
		return OwningInventory->GetOwningController();
	}

	return nullptr;
}

APawn* UItem::GetOwningPawn() const
{
	if (OwningInventory)
	{
		return OwningInventory->GetOwningPawn();
	}
	return nullptr;
}

#undef LOCTEXT_NAMESPACE