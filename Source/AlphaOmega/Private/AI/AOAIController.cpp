// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AOAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

void AAOAIController::BeginPlay()
{
	Super::BeginPlay();
	if (ensureMsgf(BTree, TEXT("Behavior Tree is nullptr! Assign BT in AI Controller")))
	{
		RunBehaviorTree(BTree);
	}

	APawn* MyPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (MyPawn)
	{
		//Pass in Move to location Vector and target actor for AI to move to
		GetBlackboardComponent()->SetValueAsVector("MoveToLocation", MyPawn->GetActorLocation());
		GetBlackboardComponent()->SetValueAsObject("TargetActor", MyPawn);
	}
}
