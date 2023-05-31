// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "InteractComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginInteract, class AAOPlayer*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndInteract, class AAOPlayer*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginFocus, class AAOPlayer*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndFocus, class AAOPlayer*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, class AAOPlayer*, Character);

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ALPHAOMEGA_API UInteractComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:

	UInteractComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact")
	float InteractTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact")
	float InteractDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact")
	FText InteractItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact")
	FText InteractActionText;

	UFUNCTION(BlueprintCallable, Category = "Interact")
	void SetInteractNameText(const FText& NewNameText);

	UFUNCTION(BlueprintCallable, Category = "Interact")
	void SetInteractActionText(const FText& NewActionText);

	//Delegates
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBeginFocus OnBeginFocus;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnEndFocus OnEndFocus;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBeginInteract OnBeginInteract;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnEndInteract OnEndInteract;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBeginInteract OnInteract;

protected:

	virtual void Deactivate() override;
	bool CanInteract(class AAOPlayer* Character) const;

	UPROPERTY()
	TArray<class AAOPlayer*> Interactors;

public:

	void RefreshWidget();

	void BeginFocus(class AAOPlayer* Character);
	void EndFocus(class AAOPlayer* Character);

	void BeginInteract(class AAOPlayer* Character);
	void EndInteract(class AAOPlayer* Character);
	void Interact(class AAOPlayer* Character);

	UFUNCTION(BlueprintPure, Category = "Interact")
	float GetInteractPercentage();
};

