// Fill out your copyright notice in the Description page of Project Settings.


#include "FX/DebuffNiagaraComp.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interfaces/CombatInterface.h"

UDebuffNiagaraComp::UDebuffNiagaraComp()
{
	bAutoActivate = false;
}

void UDebuffNiagaraComp::BeginPlay()
{
	Super::BeginPlay();

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		ASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComp::DebuffTagChanged);
	}
	if (ICombatInterface* iCombat = Cast<ICombatInterface>(GetOwner()))
	{
		iCombat->GetOnDeathDelegate().AddDynamic(this, &UDebuffNiagaraComp::OnOwnerDeath);
	}
}

void UDebuffNiagaraComp::DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		Activate();
	}
	else
	{
		Deactivate();
	}
}

void UDebuffNiagaraComp::OnOwnerDeath(AActor* DeadActor)
{
	Deactivate();
}
