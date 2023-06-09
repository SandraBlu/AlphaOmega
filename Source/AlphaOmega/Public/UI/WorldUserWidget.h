// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WorldUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UWorldUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(meta = (BindWidget))
	class USizeBox* ParentSizeBox;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

	UPROPERTY(EditAnywhere, Category = "UI")
	FVector WorldOffset;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	AActor* AttachedActor;
};
