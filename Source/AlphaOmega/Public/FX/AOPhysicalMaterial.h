// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "AOPhysicalMaterial.generated.h"

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UAOPhysicalMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	class USoundBase* FootstepSFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	class UNiagaraSystem* FootstepVFX = nullptr;
	
};