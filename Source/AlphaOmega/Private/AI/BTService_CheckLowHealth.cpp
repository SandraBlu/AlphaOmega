// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_CheckLowHealth.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "AIController.h"


UBTService_CheckLowHealth::UBTService_CheckLowHealth()
{
	LowHealthPercent = .5f;
}

void UBTService_CheckLowHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	//Get Pawn through Behavior Tree Component
	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (IsValid(AIPawn))
	{
		
// 		if (IsValid(AttributeComp))
// 		{
// 			bool bHasLowHealth = (AttributeComp->GetHealth() / AttributeComp->GetMaxHealth()) < LowHealthPercent;
// 
// 			UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
// 			BBComp->SetValueAsBool(LowHealthKey.SelectedKeyName, bHasLowHealth);
// 		}
	}
}
