// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/AnimInstanceBase.h"

#include "KismetAnimationLibrary.h"
#include "Character/BaseCharacter.h"
#include "Character/PlayerCharacter.h"
#include "GAS/AbilitySystemLibrary.h"

void UAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwningCharacter = Cast<ABaseCharacter>(TryGetPawnOwner());
	if (OwningCharacter)
	{
		OwningMovementComponent = OwningCharacter->GetCharacterMovement();
		OwningPlayerCharacter = Cast<APlayerCharacter>(OwningCharacter);
	}
}

void UAnimInstanceBase::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (!OwningCharacter || !OwningMovementComponent)
	{
		return;
	}
	if (bHasAcceleration)
	{
		IdleElapsedTime = 0.f;
		bEnterRelaxState = false;
	}
	else
	{
		IdleElapsedTime += DeltaSeconds;
		bEnterRelaxState = (IdleElapsedTime >= EnterRelaxStateThreshold);
	}
}

void UAnimInstanceBase::HandleUpdatedTarget(AActor* NewTargetActor)
{
	bIsTargeting = IsValid(NewTargetActor);
}

void UAnimInstanceBase::UpdateDirection()
{
	APawn* OwningPawn {TryGetPawnOwner()};
	if (!IsValid (OwningPawn)) { return; }
	if (!bIsTargeting) { return; }

	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(OwningPawn->GetVelocity(), OwningPawn->GetActorRotation());
}

bool UAnimInstanceBase::DoesOwnerHaveTag(FGameplayTag TagToCheck) const
{
	if (APawn* OwningPawn = TryGetPawnOwner())
	{
		return UAbilitySystemLibrary::NativeDoesActorHaveTag(OwningPawn,TagToCheck);
	}

	return false;
}
