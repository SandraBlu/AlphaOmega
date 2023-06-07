// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/AOFireProjectile.h"
#include "Projectiles/AOProjectile.h"
#include "Components/SphereComponent.h"
#include "Attributes/AOPlayerAttributeSet.h"
#include "Framework/AOBFL.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/NPCHealthComponent.h"

AAOFireProjectile::AAOFireProjectile()
{
	SphereComp->SetSphereRadius(20.0f);
	InitialLifeSpan = 10.0f;
	//DamageAmount = -0.0f;
}

// void AAOFireProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
// {
// 	if (OtherActor && OtherActor != GetInstigator())
// 	{
// 		UNPCHealthComponent* HealthComp = Cast<UNPCHealthComponent>(OtherActor->GetComponentByClass(UNPCHealthComponent::StaticClass()));
// 		if (HealthComp)
// 		{
// 			HealthComp->ApplyHealthChange(this, -DamageAmount);
// 
// 			Destroy();
// 		}
// 		//Explode();//Apply Damage & Impulse
// 	}
// }

void AAOFireProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AAOFireProjectile::OnActorOverlap);
}
