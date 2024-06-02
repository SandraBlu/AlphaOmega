// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/CharacterStates.h"
#include "AOFootstepsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALPHAOMEGA_API UAOFootstepsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UAOFootstepsComponent();

	void HandleFootstep(EFoot Foot);

protected:
	
	UPROPERTY(EditDefaultsOnly)
	FName LeftFootSocket = TEXT("foot_l");

	UPROPERTY(EditDefaultsOnly)
	FName RightFootSocket = TEXT("foot_r");	


		
};
