// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InteractableComponent.h"

#define LOCTEXT_NAMESPACE "InteractableComponent"

UInteractableComponent::UInteractableComponent()
{
	
	InteractTime = 0.f;
	InteractDistance = 200.f;
	InteractableItemName = LOCTEXT("InteractNameText", "Interact Object");
	InteractableActionText = LOCTEXT("InteractActionText", "Interact");
	
	auto FocusedOverlayFinder = ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("/Script/Engine.Material'/Game/Inventory/M_ItemPulse.M_ItemPulse'"));
	if (FocusedOverlayFinder.Succeeded())
	{
		FocusedOverlayMaterial = FocusedOverlayFinder.Object;
	}

	SetAutoActivate(true);
	SetComponentTickEnabled(false);
}

void UInteractableComponent::Deactivate()
{
	Super::Deactivate();
}

void UInteractableComponent::SetInteractableNameText(const FText& NewNameText)
{
	InteractableItemName = NewNameText;
	OnDetailChange.Broadcast();
}

void UInteractableComponent::SetInteractableActionText(const FText& NewActionText)
{
	InteractableActionText = NewActionText;
	OnDetailChange.Broadcast();
}

void UInteractableComponent::BeginFocus(class APawn* Interactor, class UInteractComponent* InteractComp)
{
	if (!IsActive() || !GetOwner() || !Interactor)
	{
		return;
	}

	OnBeginFocus.Broadcast(Interactor, InteractComp);

	if (GetNetMode() != NM_DedicatedServer)
	{
		TInlineComponentArray<UMeshComponent*> Meshes;
		GetOwner()->GetComponents(Meshes);

		for (auto& Mesh : Meshes)
		{
			if (Mesh)
			{
				Mesh->SetOverlayMaterial(FocusedOverlayMaterial);
			}
		}
	}
}

void UInteractableComponent::EndFocus(class APawn* Interactor, class UInteractComponent* InteractComp)
{
	OnEndFocus.Broadcast(Interactor, InteractComp);

	if (GetNetMode() != NM_DedicatedServer)
	{
		TInlineComponentArray<UMeshComponent*> Meshes;
		GetOwner()->GetComponents(Meshes);

		for (auto& Mesh : Meshes)
		{
			if (Mesh)
			{
				Mesh->SetOverlayMaterial(nullptr);
			}
		}
	}
}

void UInteractableComponent::BeginInteract(class APawn* Interactor, class UInteractComponent* InteractComp)
{
	OnBeginInteract(Interactor, InteractComp);
	OnBeginInteracted.Broadcast(Interactor, InteractComp);
}

void UInteractableComponent::EndInteract(class APawn* Interactor, class UInteractComponent* InteractComp)
{
	OnEndInteract(Interactor, InteractComp);
	OnEndInteracted.Broadcast(Interactor, InteractComp);
}

bool UInteractableComponent::Interact(class APawn* Interactor, class UInteractComponent* InteractComp)
{
	FText ErrorMessage;

	const bool bCanInteract = CanInteract(Interactor, InteractComp, ErrorMessage);

	if (bCanInteract)
	{
		OnInteract(Interactor, InteractComp);
		OnInteracted.Broadcast(Interactor, InteractComp);
	}

	return bCanInteract;
}

void UInteractableComponent::OnInteract_Implementation(class APawn* Interactor, class UInteractComponent* InteractComp)
{
}

void UInteractableComponent::OnBeginInteract_Implementation(class APawn* Interactor, class UInteractComponent* InteractComp)
{
}

void UInteractableComponent::OnEndInteract_Implementation(class APawn* Interactor, class UInteractComponent* InteractComp)
{
}

bool UInteractableComponent::CanInteract_Implementation(class APawn* Interactor, class UInteractComponent* InteractComp,
	FText& OutErrorText)
{
	return true;
}

#undef LOCTEXT_NAMESPACE
// Called when the game starts