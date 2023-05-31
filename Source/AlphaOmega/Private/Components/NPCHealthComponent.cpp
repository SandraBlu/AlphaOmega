// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/NPCHealthComponent.h"

// Sets default values for this component's properties
UNPCHealthComponent::UNPCHealthComponent()
{
	
	Health = 100.f;
	Health = MaxHealth;
}

UNPCHealthComponent* UNPCHealthComponent::GetAttributes(AActor* FromActor)
{
	if (FromActor)
	{
		return FromActor->FindComponentByClass<UNPCHealthComponent>();
		//return Cast<USAttributeComponent>(FromActor->GetComponentByClass(USAttributeComponent::StaticClass()));
	}

	return nullptr;
}

bool UNPCHealthComponent::IsActorAlive(AActor* Actor)
{
	UNPCHealthComponent* HealthComp = GetAttributes(Actor);
	if (HealthComp)
	{
		return HealthComp->IsAlive();
	}

	return false;
}

bool UNPCHealthComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetHealthMax());
}

bool UNPCHealthComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool UNPCHealthComponent::IsFullHealth() const
{
	return Health == MaxHealth;
}

float UNPCHealthComponent::GetHealth() const
{
	return Health;
}

float UNPCHealthComponent::GetHealthMax() const
{
	return MaxHealth;
}

bool UNPCHealthComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	Health += Delta;
	return true;
}

