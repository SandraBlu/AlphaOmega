// Fill out your copyright notice in the Description page of Project Settings.


#include "OAssetManager.h"

#include "OGameplayTags.h"

UOAssetManager& UOAssetManager::Get()
{
	check(GEngine)
	UOAssetManager* RAssetManager = Cast<UOAssetManager>(GEngine->AssetManager);
	return *RAssetManager;
}

void UOAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FOGameplayTags::InitializeNativeGameplayTags();
}
