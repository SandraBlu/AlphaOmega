// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectiles/AOProjectile.h"
#include "GameplayTagContainer.h"
#include "AOFireProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API AAOFireProjectile : public AAOProjectile
{
	GENERATED_BODY()
	
public:
	AAOFireProjectile();

	protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageAmount;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag ParryTag;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	class UGameplayEffect* AbilityEffectClass;

	UFUNCTION(BlueprintImplementableEvent)
	void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void PostInitializeComponents() override;
};
