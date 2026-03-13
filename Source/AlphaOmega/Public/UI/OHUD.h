// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "OHUD.generated.h"

class UAttributeSet;
struct FWidgetControllerParams;
class UAbilitySystemComponent;
class UAbilityMenuController;
class UAttributeMenuController;
class UOverlayWidgetController;
class UOUserWidget;
/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API AOHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	UAttributeMenuController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);
	UAbilityMenuController* GetAbilityMenuWidgetController(const FWidgetControllerParams& WCParams);
	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

private:
	
	UPROPERTY()
	UOUserWidget* OverlayWidget;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UOUserWidget> OverlayWidgetClass;
	
	UPROPERTY()
	UOverlayWidgetController* OverlayWidgetController;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY()
	UAttributeMenuController* AttributeMenuController;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuController> AttributeMenuControllerClass;

	UPROPERTY()
	UAbilityMenuController* AbilityMenuController;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAbilityMenuController> AbilityMenuControllerClass;
	
};
