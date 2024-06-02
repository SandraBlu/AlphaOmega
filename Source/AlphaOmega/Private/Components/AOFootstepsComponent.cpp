// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AOFootstepsComponent.h"
#include "Character/AOCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "FX/AOPhysicalMaterial.h"
#include "Character/CharacterStates.h"

// Sets default values for this component's properties
UAOFootstepsComponent::UAOFootstepsComponent()
{	
	PrimaryComponentTick.bCanEverTick = true;
}

void UAOFootstepsComponent::HandleFootstep(EFoot Foot)
{
	if (AAOCharacter* Character = Cast<AAOCharacter>(GetOwner()))
	{
		if (USkeletalMeshComponent* Mesh = Character->GetMesh())
		{
			FHitResult HitResult;
			const FVector SocketLocation = Mesh->GetSocketLocation(Foot == EFoot::Left ? LeftFootSocket : RightFootSocket);
			const FVector Location = SocketLocation + FVector::UpVector * 20;

			FCollisionQueryParams QueryParams;
			QueryParams.bReturnPhysicalMaterial = true;
			QueryParams.AddIgnoredActor(Character);

			if (GetWorld()->LineTraceSingleByChannel(HitResult, Location, Location + FVector::UpVector * -50.f, ECollisionChannel::ECC_WorldStatic, QueryParams))
			{
				if (HitResult.bBlockingHit)
				{
					if (HitResult.PhysMaterial.Get())
					{
						UAOPhysicalMaterial* PhysicsMat = Cast<UAOPhysicalMaterial>(HitResult.PhysMaterial.Get());

 						if (PhysicsMat)
 						{
 							UGameplayStatics::PlaySoundAtLocation(this, PhysicsMat->FootstepSFX, Location, 1.f);
 							UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PhysicsMat->FootstepVFX, Location);
 						}
					}
				}
			}
		}
	}
}
