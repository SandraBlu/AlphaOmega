// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/FootstepComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "Character/PlayerCharacter.h"
#include "Framework/OPhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/EnumTypes.h"


// Sets default values for this component's properties
UFootstepComponent::UFootstepComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFootstepComponent::HandleFootstep(EFoot Foot)
{
	if (APlayerCharacter* Character = Cast<APlayerCharacter>(GetOwner()))
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
						UOPhysicalMaterial* PhysicsMat = Cast<UOPhysicalMaterial>(HitResult.PhysMaterial.Get());

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
