// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Controllers/AOWidgetController.h"

void UAOWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UAOWidgetController::BroadcastInitialValues()
{

}

void UAOWidgetController::BindCallbacksToDependencies()
{

}
