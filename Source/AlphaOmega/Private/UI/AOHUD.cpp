// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AOHUD.h"
#include "UI/AOUserWidget.h"
#include "UI/Controllers/OverlayController.h"
#include "UI/Controllers/AOAttributeMenuController.h"
#include "UI/Controllers/AbilityMenuController.h"

UOverlayController* AAOHUD::GetOverlayController(const FWidgetControllerParams& WCParams)
{
	if (OverlayController == nullptr)
	{
		OverlayController = NewObject<UOverlayController>(this, OverlayControllerClass);
		OverlayController->SetWidgetControllerParams(WCParams);
		OverlayController->BindCallbacksToDependencies();
	}
	return OverlayController;
}

UAOAttributeMenuController* AAOHUD::GetAttributeMenuController(const FWidgetControllerParams& WCParams)
{
	if (AttributeMenuController == nullptr)
	{
		AttributeMenuController = NewObject<UAOAttributeMenuController>(this, AttributeMenuControllerClass);
		AttributeMenuController->SetWidgetControllerParams(WCParams);
		AttributeMenuController->BindCallbacksToDependencies();
	}
	return AttributeMenuController;
}

UAbilityMenuController* AAOHUD::GetAbilityMenuController(const FWidgetControllerParams& WCParams)
{
	if (AbilityMenuController == nullptr)
	{
		AbilityMenuController = NewObject<UAbilityMenuController>(this, AbilityMenuControllerClass);
		AbilityMenuController->SetWidgetControllerParams(WCParams);
		AbilityMenuController->BindCallbacksToDependencies();
	}
	return AbilityMenuController;
}

void AAOHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class unintialized, please fill out BP_AOHUD"));
	checkf(OverlayControllerClass, TEXT("Overlay Controller Class unintialized, please fill out BP_AOHUD"));

	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UAOUserWidget>(Widget);

	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	UOverlayController* WidgetController = GetOverlayController(WidgetControllerParams);

	OverlayWidget->SetWidgetController(WidgetController);
	WidgetController->BroadcastInitialValues();
	Widget->AddToViewport();
}
