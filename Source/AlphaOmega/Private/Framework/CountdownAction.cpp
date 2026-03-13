// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CountdownAction.h"

void FCountDownAction::UpdateOperation(FLatentResponse& Response)
{
	if (bNeedToCancel)
	{
		CountDownOutput = ECountDownActionOutput::Cancelled;
		Response.FinishAndTriggerIf(true,ExecutionFunction,OutputLink,CallbackTarget);
		return;
	}
	if (ElapsedTimeSinceStart >= TotalCountDownTime)
	{
		CountDownOutput = ECountDownActionOutput::Completed;
		Response.FinishAndTriggerIf(true,ExecutionFunction,OutputLink,CallbackTarget);
		return;
	}
	if (ElapsedInterval < UpdateInterval)
	{
		ElapsedInterval += Response.ElapsedTime();
	}
	else
	{
		ElapsedTimeSinceStart += UpdateInterval > 0.f? UpdateInterval : Response.ElapsedTime();
		OutRemainingTime = TotalCountDownTime - ElapsedTimeSinceStart;
		CountDownOutput = ECountDownActionOutput::Updated;
		Response.TriggerLink(ExecutionFunction,OutputLink,CallbackTarget);
		  
		ElapsedInterval = 0.f;
	}
}

void FCountDownAction::CancelAction()
{
	bNeedToCancel = true;
}
