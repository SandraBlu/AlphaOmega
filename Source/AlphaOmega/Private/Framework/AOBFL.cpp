// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/AOBFL.h"
#include "Components/AOCoreComponent.h"
#include "Attributes/AOPlayerAttributeSet.h"


UAOCoreComponent* UAOBFL::GetAOCoreComponent(const AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return nullptr;
	}
	// Fall back to a component search to better support BP-only actors
	return Actor->FindComponentByClass<UAOCoreComponent>();
}

