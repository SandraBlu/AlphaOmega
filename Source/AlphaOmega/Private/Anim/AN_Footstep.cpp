// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/AN_Footstep.h"

#include "AI/NPCBase.h"
#include "Character/PlayerCharacter.h"
#include "Components/FootstepComponent.h"

class APlayerCharacter;

void UAN_Footstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,  const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	check(MeshComp);

	APlayerCharacter* Character = MeshComp ? Cast<APlayerCharacter>(MeshComp->GetOwner()) : nullptr;
	if (Character)
	{
		if (UFootstepComponent* FootstepsComp = Character->GetFootstepsComp())
		{
			FootstepsComp->HandleFootstep(Foot);
		}
	}
}
