// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/AOUseItem.h"

#define LOCTEXT_NAMESPACE "Use Item"
UAOUseItem::UAOUseItem()
{
	HealAmount = 30.f;
	UseActionText = LOCTEXT("ItemUseActionText", "Consume");
}

void UAOUseItem::Use(class AAOPlayer* Character)
{
	UE_LOG(LogTemp, Warning, TEXT("Used Item"));
}

#undef LOCTEXT_NAMESPACE