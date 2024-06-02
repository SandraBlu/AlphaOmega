// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AOInventoryBFL.generated.h"

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UAOInventoryBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory", meta = (DefaultToSelf = "Target"))
	static class UAOInventoryComponent* GetInventoryComponentFromTarget(AActor* Target);

	
	//Sort the array of inventory items from a-z
	UFUNCTION(BlueprintPure, Category = "Inventory")
	static TArray<class UAOItem*> SortItemArrayAlphabetical(TArray<class UAOItem*> InItems, const bool bReverse);


	//Sort the array of inventory items using stack weight
	UFUNCTION(BlueprintPure, Category = "Inventory")
	static TArray<class UAOItem*> SortItemArrayWeight(TArray<class UAOItem*> InItems, const bool bReverse);
	
};
