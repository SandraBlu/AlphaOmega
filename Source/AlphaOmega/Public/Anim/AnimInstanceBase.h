// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimInstanceBase.generated.h"

class UCharacterMovementComponent;
class ABaseCharacter;
/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()

public:
	
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds);

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	void HandleUpdatedTarget(AActor* NewTargetActor);
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	void UpdateDirection();
	
protected:
	UPROPERTY()
	ABaseCharacter* OwningCharacter;
	
	UPROPERTY()
	UCharacterMovementComponent* OwningMovementComponent;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "AnimData|LocomotionData")
	float GroundSpeed;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bHasAcceleration;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "AnimData|LocomotionData")
	float LocomotionDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimData|LocomotionData")
	bool bIsTargeting{false};

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool DoesOwnerHaveTag(FGameplayTag TagToCheck) const;

//player
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|References")
	class APlayerCharacter* OwningPlayerCharacter;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|References")
	bool bEnterRelaxState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|References")
	float EnterRelaxStateThreshold = 5.f;
	
	float IdleElapsedTime;
	
};
