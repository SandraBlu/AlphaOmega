// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Credit.generated.h"

class AAOPlayerController;

UCLASS()
class ALPHAOMEGA_API ACredit : public AActor
{
	GENERATED_BODY()
	

public:

	ACredit();

	UPROPERTY(VisibleAnywhere)
	AAOPlayerController* PC;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category  = "Components")
	class UStaticMeshComponent* CreditMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category  = "Components")
	class UInteractComponent* CreditInteract;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category  = "Components")
	class UAOInventoryComponent* Inventory;

protected:

	UPROPERTY(EditAnywhere, Category = "Credit")
	int32 CreditAmount;
};
