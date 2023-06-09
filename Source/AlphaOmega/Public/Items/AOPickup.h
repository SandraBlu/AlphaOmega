// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AOPickup.generated.h"

UCLASS()
class ALPHAOMEGA_API AAOPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AAOPickup();

	
	//Creates pickup from template. Done on Begin play when player drops item on ground
	void InitializePickup(const TSubclassOf<class UAOItem> ItemClass, const int32 Quantity);

	//Align pickups with ground
	UFUNCTION(BlueprintImplementableEvent)
	void AlignWithGround();

	//Template to create pickup when spawned in
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	class UAOItem* ItemTemplate;

protected:

	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UAOItem* Item;

	UFUNCTION()
	void UpdateItemSpecs();
	//void OnRep_Item();

	UFUNCTION()
	void OnItemModified();

	virtual void BeginPlay() override;

	#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

UFUNCTION()
	void OnTakePickup(class AAOPlayer* Taker);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* PickupMesh;

	UPROPERTY(EditAnywhere, Category = "VFX")
	class UNiagaraComponent* PickupVFX;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UInteractComponent* InteractComp;

};
