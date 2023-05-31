// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AOInteractWidget.h"
#include "Components/InteractComponent.h"


void UAOInteractWidget::UpdateInteractWidget(class UInteractComponent* InteractComp)
{
	OwningInteractComp = InteractComp;
	OnUpdateInteractWidget();
}

