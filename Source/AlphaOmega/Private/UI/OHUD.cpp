// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OHUD.h"

#include "UI/GAS/OUserWidget.h"
#include "UI/GAS/Controller/AbilityMenuController.h"
#include "UI/GAS/Controller/AttributeMenuController.h"
#include "UI/GAS/Controller/OverlayWidgetController.h"

UOverlayWidgetController* AOHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController ==nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallbacksToDependencies();
	}
	return OverlayWidgetController;
}

UAttributeMenuController* AOHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (AttributeMenuController == nullptr)
	{
		AttributeMenuController = NewObject<UAttributeMenuController>(this, AttributeMenuControllerClass);
		AttributeMenuController->SetWidgetControllerParams(WCParams);
		AttributeMenuController->BindCallbacksToDependencies();
	}
	return AttributeMenuController;
}

UAbilityMenuController* AOHUD::GetAbilityMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (AbilityMenuController == nullptr)
	{
		AbilityMenuController = NewObject<UAbilityMenuController>(this, AbilityMenuControllerClass);
		AbilityMenuController->SetWidgetControllerParams(WCParams);
		AbilityMenuController->BindCallbacksToDependencies();
	}
	return AbilityMenuController;
}

void AOHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitialized, please fill out BP_HUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class uninitialized, please fill out BP_HUD"));

	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UOUserWidget>(Widget);

	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);

	OverlayWidget->SetWidgetController(WidgetController);
	WidgetController->BroadcastInitialValues();
	Widget->AddToViewport();
}