// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectiles/AOProjectile.h"
#include "AOMissile.generated.h"

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API AAOMissile : public AAOProjectile
{
	GENERATED_BODY()

	AAOMissile();

	UFUNCTION()
	void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void PostInitializeComponents() override;
	
};
