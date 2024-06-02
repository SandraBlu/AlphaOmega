// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Heal.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_Heal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* AIPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (AIPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

// 	if (IsValid(AttributeComp))
// 	{
// 	//	AttributeComp->ApplyHealthChange(AIPawn, AttributeComp->GetMaxHealth());
// 	}

	return EBTNodeResult::Succeeded;
}
