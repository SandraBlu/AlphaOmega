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

	HealthComp = CreateDefaultSubobject<UNPCHealthComponent>("HealthComp");

	LootInteract = CreateDefaultSubobject<UInteractComponent>("LootInteract");
	LootInteract->InteractActionText = LOCTEXT("LootAIText", "Loot");
	LootInteract->InteractItemName = LOCTEXT("LootAIName", "AIName");
	LootInteract->SetupAttachment(GetRootComponent());
	LootInteract->SetActive(false, true);
	LootInteract->bAutoActivate = false;
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
	HealthComp->OnHealthChanged.AddDynamic(this, &AAICharacter::OnHealthChanged);
}

void AAICharacter::OnHealthChanged(AActor* InstigatorActor, UNPCHealthComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.0f)
	{
		if (InstigatorActor != this)
		{
			SetTargetActor(InstigatorActor);
		}

		if (HealthBar == nullptr)
		{
			HealthBar = CreateWidget<UWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
			if (HealthBar)
			{
				HealthBar->AttachedActor = this;
				HealthBar->AddToViewport();
			}
		}

		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);

		// Died
		if (NewHealth <= 0.0f)
		{
			// stop BT
			AAIController* AIC = Cast<AAIController>(GetController());
			if (AIC)
			{
				AIC->GetBrainComponent()->StopLogic("Killed");
			}

			// ragdoll
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetMesh()->SetCollisionProfileName("Ragdoll");

			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCharacterMovement()->DisableMovement();

			// set lifespan
			SetLifeSpan(10.0f);
		}
	}
}

void AAICharacter::OnPawnSeen(APawn* Pawn)
{
	SetTargetActor(Pawn);

	DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::Green, 4.f, true);
}

#undef LOCTEXT_NAMESPACE