// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/AORestoreBase.h"
#include "Components/InteractComponent.h"
#include "NiagaraComponent.h"
#include <Components/StaticMeshComponent.h>

// Sets default values
AAORestoreBase::AAORestoreBase()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	SetRootComponent(MeshComp);

	RestoreSiteVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RestoreSite"));
	RestoreSiteVFX->SetupAttachment(GetRootComponent());

	InteractComp = CreateDefaultSubobject<UInteractComponent>("RestoreInteractComp");
	InteractComp->InteractTime = 5.f;
	InteractComp->InteractDistance = 750.f;
	InteractComp->InteractItemName = FText::FromString("RestoreSite");
	InteractComp->InteractActionText = FText::FromString("Heal");
	InteractComp->SetupAttachment(MeshComp);

	RespawnTime = 5.f;

}

void AAORestoreBase::IsActive()
{
	// Set visibility on root and all children
	RootComponent->SetVisibility(bIsActive, true);
}

void AAORestoreBase::ShowRestoreSite()
{
	SetRestoreSiteState(true);
}

void AAORestoreBase::HideAndCooldownRestoreSite()
{
	SetRestoreSiteState(false);

	GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &AAORestoreBase::ShowRestoreSite, RespawnTime);
}

void AAORestoreBase::SetRestoreSiteState(bool bNewIsActive)
{
	SetActorEnableCollision(bNewIsActive);

	//Set Visibility on root and all children
	RootComponent->SetVisibility(bNewIsActive, true);
}


