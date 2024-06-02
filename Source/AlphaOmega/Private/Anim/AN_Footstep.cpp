// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/AN_Footstep.h"
#include "Character/AOCharacter.h"
#include "Components/AOFootstepsComponent.h"

void UAN_Footstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	check(MeshComp);

	AAOCharacter* Character = MeshComp ? Cast<AAOCharacter>(MeshComp->GetOwner()) : nullptr;
	if (Character)
	{
		if (UAOFootstepsComponent* FootstepsComp = Character->GetFootstepsComp())
		{
			FootstepsComp->HandleFootstep(Foot);
		}
	}
}
