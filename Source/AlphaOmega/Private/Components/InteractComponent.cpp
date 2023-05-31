// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InteractComponent.h"
#include "Character/AOPlayer.h"
#include "UI/AOInteractWidget.h"

UInteractComponent::UInteractComponent()
{
	SetComponentTickEnabled(false);

	InteractTime = 0.f;
	InteractDistance = 1000.f;
	InteractItemName = FText::FromString("Interactable Object");
	InteractActionText = FText::FromString("Interact");

	Space = EWidgetSpace::Screen;
	DrawSize = FIntPoint(400, 100);
	bDrawAtDesiredSize = true;

	SetActive(true);
	SetHiddenInGame(true);
}

void UInteractComponent::SetInteractNameText(const FText& NewNameText)
{
	InteractItemName = NewNameText;
	RefreshWidget();
}

void UInteractComponent::SetInteractActionText(const FText& NewActionText)
{
	InteractActionText = NewActionText;
	RefreshWidget();
}



void UInteractComponent::Deactivate()
{
	Super::Deactivate();

	for (int32 i = Interactors.Num() - 1; i >= 0; --i)
	{
		if (AAOPlayer* Interactor = Interactors[i])
		{
			EndFocus(Interactor);
			EndInteract(Interactor);
		}
	}
	Interactors.Empty();
}

bool UInteractComponent::CanInteract(class AAOPlayer* Character) const
{
	return IsActive() && GetOwner() != nullptr && Character != nullptr;
}


 void UInteractComponent::RefreshWidget()
 {
 	if (!bHiddenInGame)
 	{
 		if (UAOInteractWidget* InteractWidget = Cast<UAOInteractWidget>(GetUserWidgetObject()))
 		{
 			InteractWidget->UpdateInteractWidget(this);
 		}
 	}
 }

void UInteractComponent::BeginFocus(class AAOPlayer* Character)
{

	if (!IsActive() || !GetOwner() || !Character)
	{
		return;
	}
	OnBeginFocus.Broadcast(Character);

	SetHiddenInGame(false);
	RefreshWidget();
}

void UInteractComponent::EndFocus(class AAOPlayer* Character)
{
	OnEndFocus.Broadcast(Character);

	SetHiddenInGame(true);
}

void UInteractComponent::BeginInteract(class AAOPlayer* Character)
{
	if (CanInteract(Character))
	{
		Interactors.AddUnique(Character);
		OnBeginInteract.Broadcast(Character);
	}
}

void UInteractComponent::EndInteract(class AAOPlayer* Character)
{
	Interactors.RemoveSingle(Character);
	OnEndInteract.Broadcast(Character);
}

void UInteractComponent::Interact(class AAOPlayer* Character)
{
	if (CanInteract(Character))
	{
		OnInteract.Broadcast(Character);
	}
}

float UInteractComponent::GetInteractPercentage()
{
	if (Interactors.IsValidIndex(0))
	{
		if (AAOPlayer* Interactor = Interactors[0])
		{
			if (Interactor && Interactor->IsInteracting())
			{
				return 1.f - FMath::Abs(Interactor->GetRemainingInteractTime() / InteractTime);
			}
		}
	}
	return 0.f;
}