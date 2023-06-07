// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AORestoreBase.generated.h"

UCLASS()
class ALPHAOMEGA_API AAORestoreBase : public AActor
{
	GENERATED_BODY()
	
public:	

	AAORestoreBase();

protected:

	UPROPERTY()
	bool bIsActive;

	UFUNCTION()
	void IsActive();

	UPROPERTY(EditAnywhere, Category = "Powerup")
	float RespawnTime;

	FTimerHandle TimerHandle_RespawnTimer;

	UFUNCTION()
	void ShowRestoreSite();

	void HideAndCooldownRestoreSite();

	void SetRestoreSiteState(bool bNewIsActive);

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class USphereComponent> SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> MeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UInteractComponent* InteractComp;
	UPROPERTY(EditAnywhere, Category = "VFX")
	class UNiagaraComponent* RestoreSiteVFX;

};
