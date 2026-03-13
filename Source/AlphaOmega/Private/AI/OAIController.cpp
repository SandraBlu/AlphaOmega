// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/OAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AOAIController::AOAIController()
{
	AISightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("AISightConfig");
	AISightConfig->DetectionByAffiliation.bDetectEnemies = true;
	AISightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	AISightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	AISightConfig->SightRadius = 5000.0f;
	AISightConfig->LoseSightRadius = 0.f;
	AISightConfig->PeripheralVisionAngleDegrees = 180.f;

	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>("PerceptionComp");
	PerceptionComp->ConfigureSense(*AISightConfig);
	PerceptionComp->SetDominantSense(UAISenseConfig_Sight::StaticClass());
	PerceptionComp->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::OnEnemyPerceptionUpdated);

	SetGenericTeamId(FGenericTeamId(1));
	
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComp");
	check(Blackboard);
	BTree = CreateDefaultSubobject<UBehaviorTree>("BehaviorTree");
	check(BTree);
}

ETeamAttitude::Type AOAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const APawn* PawnToCheck = Cast<const APawn>(&Other);
	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(PawnToCheck->GetController());
	if (OtherTeamAgent && OtherTeamAgent->GetGenericTeamId() < GetGenericTeamId())
	{
		return ETeamAttitude::Hostile;
	}
	return ETeamAttitude::Friendly;
}

void AOAIController::BeginPlay()
{
	Super::BeginPlay();
	
	if (UCrowdFollowingComponent* CrowdComp = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		CrowdComp->SetCrowdSimulationState(bEnableCrowdAvoidance? ECrowdSimulationState::Enabled : ECrowdSimulationState::Disabled);
		switch (CrowdAvoidanceQuality)
		{
		case 1: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Low);    break;
		case 2: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Medium); break;
		case 3: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Good);   break;
		case 4: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);   break;
		default:
			break;
		}
		CrowdComp->SetAvoidanceGroup(1);
		CrowdComp->SetGroupsToAvoid(1);
		CrowdComp->SetCrowdCollisionQueryRange(CollisionQueryRange);
	}
}

void AOAIController::OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
	{
		if (!BlackboardComp->GetValueAsObject(FName("TargetActor")))
		{
			if (Stimulus.WasSuccessfullySensed() && Actor)
			{
				BlackboardComp->SetValueAsObject(FName("TargetActor"), Actor);
			}
		}
	}
}