// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AOPlayerController.generated.h"

class UAODamageTextComp;

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API AAOPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	

	UFUNCTION()
	void ShowDamageText(float DamageText, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);


private:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAODamageTextComp> DamageTextCompClass;
};
