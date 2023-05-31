// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Credit.h"
#include "Components/InteractComponent.h"
#include "Framework/AOPlayerController.h"

#define LOCTEXT_NAMESPACE "Credit"

ACredit::ACredit()
{
	CreditMesh = CreateDefaultSubobject<UStaticMeshComponent>("CreditMesh");
	SetRootComponent(CreditMesh);

	CreditInteract = CreateDefaultSubobject<UInteractComponent>("CreditInteract");
	CreditInteract->InteractActionText = LOCTEXT("CreditText", "Credit");
	CreditInteract->InteractItemName = LOCTEXT("CreditName", "Cache");
	CreditInteract->SetupAttachment(GetRootComponent());

	CreditAmount = 25;
}

#undef LOCTEXT_NAMESPACE