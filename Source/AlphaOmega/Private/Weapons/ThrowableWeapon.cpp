// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ThrowableWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AThrowableWeapon::AThrowableWeapon()
{
    ThrowableMesh = CreateDefaultSubobject<UStaticMeshComponent>("ThrowableMesh");
    SetRootComponent(ThrowableMesh);

    MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
    MovementComp->InitialSpeed = 1000.f;
}

