// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/AOItem.h"
#include "AOUseItem.generated.h"

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UAOUseItem : public UAOItem
{
	GENERATED_BODY()

public:

	UAOUseItem();

	virtual void Use(class AAOPlayer* Character) override;
	
};
