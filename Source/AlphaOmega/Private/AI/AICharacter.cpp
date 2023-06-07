// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AICharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AlphaOmega/DebugMacros.h"
#include "BrainComponent.h"
#include "Blueprint/UserWidget.h"
#include "UI/WorldUserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InteractComponent.h"
#include "Components/NPCHealthComponent.h"
#include "Framework/AOBFL.h"
#include "Components/AOCoreComponent.h"

#define LOCTEXT_NAMESPACE "LootAI"

AAICharacter::AAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

// 	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
// 	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
// 	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	LootInteract = CreateDefaultSubobject<UInteractComponent>("LootInteract");
	LootInteract->InteractActionText = LOCTEXT("LootAIText", "Loot");
	LootInteract->InteractItemName = LOCTEXT("LootAIName", "AIName");
	LootInteract->SetupAttachment(GetRootComponent());
	LootInteract->SetActive(false, true);
	LootInteract->bAutoActivate = false;

	TimeToHitParamName = "TimeToHit";
	TargetActorKey = "TargetActor";
}

void AAICharacter::SetTargetActor(AActor* NewTarget)
{
	AAIController* AIC = Cast<AAIController>(GetController());
	if (AIC)
	{
		AIC->GetBlackboardComponent()->SetValueAsObject("TargetActor", NewTarget);
	}
}


void AAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AAICharacter::OnPawnSeen);
}

void AAICharacter::OnPawnSeen(APawn* Pawn)
{
	SetTargetActor(Pawn);

	DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::Green, 4.f, true);
}

#undef LOCTEXT_NAMESPACE