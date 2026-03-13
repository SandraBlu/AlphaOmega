// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TargetActor.h"

#include "Abilities/GameplayAbility.h"
#include "Framework/OPlayerController.h"

ATargetActor::ATargetActor()
{
	PrimaryActorTick.bCanEverTick = true;
	TraceRange = 1500.0f;
	bDestroyOnConfirmation = true;
	ShouldProduceTargetDataOnServer = true;
	ReticleActor = nullptr;
}

void ATargetActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	LineTraceFN(TraceHitResults);

	if (ReticleActor)
	{
		if (TraceHitResults.bBlockingHit)
		{
			ReticleActor->SetActorLocation(TraceHitResults.ImpactPoint, false, nullptr, ETeleportType::None);
		}
		else
		{
			ReticleActor->SetActorLocation(TraceHitResults.TraceEnd, false, nullptr, ETeleportType::None);
		}
	}
}

bool ATargetActor::LineTraceFN(FHitResult& TraceHitResult) const
{
	AOPlayerController* RPC = Cast<AOPlayerController>(GetOwner()->GetInstigatorController());
	FVector ViewPoint;
	FRotator ViewRotation;
	RPC->GetPlayerViewPoint(ViewPoint, ViewRotation);
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	
	if (APawn* MasterPawn = RPC->GetPawn())
	{
		QueryParams.AddIgnoredActor(MasterPawn->GetUniqueID());
	}

	bool HitResult = GetWorld()->LineTraceSingleByChannel(TraceHitResult, ViewPoint, ViewPoint + ViewRotation.Vector() * TraceRange, ECC_Visibility, QueryParams);

	return HitResult;
}

void ATargetActor::StartTargeting(UGameplayAbility* Ability)
{
	OwningAbility = Ability;
	PrimaryPC = Cast<AOPlayerController>(Ability->GetOwningActorFromActorInfo()->GetInstigatorController());
	ReticleActor = SpawnReticleActor(GetActorLocation(), GetActorRotation());
}

void ATargetActor::ConfirmTargetingAndContinue()
{
	FHitResult HitResult;
	bool TryTrace = LineTraceFN(HitResult);
	FGameplayAbilityTargetDataHandle TargetData = StartLocation.MakeTargetDataHandleFromHitResult(OwningAbility, HitResult);
	if (TargetData != nullptr)
	{
		TargetDataReadyDelegate.Broadcast(TargetData);
	}
	else
	{
		TargetDataReadyDelegate.Broadcast(FGameplayAbilityTargetDataHandle());
	}
	DestroyReticleActors();
}

void ATargetActor::CancelTargeting()
{
	Super::CancelTargeting();
	DestroyReticleActors();
}

AGameplayAbilityWorldReticle* ATargetActor::SpawnReticleActor(FVector Location, FRotator Rotation)
{
	if (ReticleClass)
	{
		if (!ReticleActor)
		{
			if (AGameplayAbilityWorldReticle* SpawnedReticleActor = GetWorld()->SpawnActor<AGameplayAbilityWorldReticle>(ReticleClass, Location, Rotation))
			{
				AOPlayerController* RPC = Cast<AOPlayerController>(GetOwner()->GetInstigatorController());
				SpawnedReticleActor->InitializeReticle(this, RPC, ReticleParams);
				//SpawnedReticleActor->SetReplicates(false);
				return SpawnedReticleActor;
			}
		}
		else
		{
			return ReticleActor;
		}
	}
	return nullptr;
}

void ATargetActor::DestroyReticleActors()
{
	if (ReticleActor)
	{
		ReticleActor->Destroy();
	}
}
