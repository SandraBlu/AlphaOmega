// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/AOPlayerController.h"


AAOPlayerController::AAOPlayerController()
{

}

int32 AAOPlayerController::GetCredit() const
{
	return Credit;
}

void AAOPlayerController::AddCredit(int32 Delta)
{
	// Avoid user-error of adding a negative amount
	if (!ensure(Delta >= 0.0f))
	{
		return;
	}

	Credit += Delta;

	OnCreditChanged.Broadcast(this, Credit, Delta);
}

bool AAOPlayerController::RemoveCredit(int32 Delta)
{
	// Avoid user-error of adding a subtracting negative amount
	if (!ensure(Delta >= 0.0f))
	{
		return false;
	}

	if (Credit < Delta)
	{
		// Not enough credits available
		return false;
	}

	Credit -= Delta;

	OnCreditChanged.Broadcast(this, Credit, -Delta);

	return true;
}

void AAOPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	OnPawnChanged.Broadcast(InPawn);
}
