// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AOPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API AAOPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	//Inventory
	UFUNCTION(BlueprintImplementableEvent)
	void ShowInGameUI();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowNotification(const FText& Message);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowDeathScreen(class AAOCharacter* Killer);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowLootMenu(const class UAOInventoryComponent* LootSource);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HideLootMenu();
	
};
