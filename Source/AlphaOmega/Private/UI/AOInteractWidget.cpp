// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AOInteractWidget.h"
#include "Components/AOInteractComponent.h"

void UAOInteractWidget::UpdateInteractWidget(class UAOInteractComponent* InteractComp)
{
	OwningInteractComp = InteractComp;
	OnUpdateInteractWidget();
}
