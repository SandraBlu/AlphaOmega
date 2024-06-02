// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AOUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UAOUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	UObject* WidgetController;

	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);

protected:

	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerInit();
};
