// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AOPlayerController.generated.h"

//Event handler for re Spawning Character
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnChanged, APawn*, NewPawn);

// Event Handler for Credits
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditChanged, APlayerController*, PlayerController, int32, NewCredit, int32, Delta);

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API AAOPlayerController : public APlayerController
{
	GENERATED_BODY()

	AAOPlayerController();

	protected:

	UPROPERTY(EditDefaultsOnly, Category = "Credit")
	int32 Credit;

	public:

	// Credit
	UFUNCTION(BlueprintCallable, Category = "Credit")
	int32 GetCredit() const;

	UFUNCTION(BlueprintCallable, Category = "Credit")
	void AddCredit(int32 Delta);

	UFUNCTION(BlueprintCallable, Category = "Credit")
	bool RemoveCredit(int32 Delta);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCreditChanged OnCreditChanged;

	//Pawn
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPawnChanged OnPawnChanged;

	virtual void SetPawn(APawn* InPawn) override;
	
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

	UFUNCTION(BlueprintImplementableEvent)
	void OnHitPlayer();
};
