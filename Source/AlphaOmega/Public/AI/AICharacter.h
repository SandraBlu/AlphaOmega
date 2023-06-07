// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AOCharacter.h"
#include "AICharacter.generated.h"

class UNPCHealthComponent;
class UWorldUserWidget;

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API AAICharacter : public AAOCharacter
{
	GENERATED_BODY()

public:

	AAICharacter();

protected:

	/* Widget to display when bot first sees a player. */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SpottedWidgetClass;

	/* Material parameter for Hitflashes */
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TimeToHitParamName;

	/* Key for AI Blackboard 'TargetActor' */
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TargetActorKey;

	UFUNCTION()
	void SetTargetActor(AActor* NewTarget);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class UInteractComponent* LootInteract;

	virtual void PostInitializeComponents();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UPawnSensingComponent* PawnSensingComp;

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);
	
};
