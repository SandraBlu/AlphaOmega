// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GAS/OUserWidget.h"

void UOUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
