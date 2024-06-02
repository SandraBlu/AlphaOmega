// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/AOPlayerController.h"
#include "UI/AODamageTextComp.h"
#include "GameFramework/Character.h"

void AAOPlayerController::ShowDamageText(float DamageText, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	if (IsValid(TargetCharacter) && DamageTextCompClass)
	{
		UAODamageTextComp* DamageComp = NewObject<UAODamageTextComp>(TargetCharacter, DamageTextCompClass);
		DamageComp->RegisterComponent();
		DamageComp->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageComp->SetDamageText(DamageText, bBlockedHit, bCriticalHit);
	}
}
