// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AOHUD.generated.h"

class UAOUserWidget;
class UOverlayController;
struct FWidgetControllerParams;
class APlayerController;
class APlayerState;
class UAbilitySystemComponent;
class UAttributeSet;
class UAOAttributeMenuController;
class UAbilityMenuController;
/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API AAOHUD : public AHUD
{
	GENERATED_BODY()

public:

	UOverlayController* GetOverlayController(const FWidgetControllerParams& WCParams);
	UAOAttributeMenuController* GetAttributeMenuController(const FWidgetControllerParams& WCParams);
	UAbilityMenuController* GetAbilityMenuController(const FWidgetControllerParams& WCParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

private:

	UPROPERTY()
	UAOUserWidget* OverlayWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAOUserWidget> OverlayWidgetClass;

	UPROPERTY()
	UOverlayController* OverlayController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayController> OverlayControllerClass;

	UPROPERTY()
	UAOAttributeMenuController* AttributeMenuController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAOAttributeMenuController> AttributeMenuControllerClass;

	UPROPERTY()
	UAbilityMenuController* AbilityMenuController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAbilityMenuController> AbilityMenuControllerClass;

	
};
