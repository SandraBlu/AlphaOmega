// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/AOMissile.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Framework/AOBFL.h"

AAOMissile::AAOMissile()
{
	SphereComp->SetSphereRadius(20.0f);
}

void AAOMissile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	AActor* SourceAvatarActor = DamageEffectParams.SourceASC->GetAvatarActor();
	if  (SourceAvatarActor == OtherActor) return;
	//if (!UAOBFL::IsNotFriend(SourceAvatarActor, OtherActor)) return;
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactVFX, GetActorLocation());
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());

	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		DamageEffectParams.TargetASC = TargetASC;
		UAOBFL::ApplyDamageEffect(DamageEffectParams);
	}
	//Destroy();
}

void AAOMissile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// More consistent to bind here compared to Constructor which may fail to bind if Blueprint was created before adding this binding (or when using hotreload)
	// PostInitializeComponent is the preferred way of binding any events.
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AAOMissile::OnActorOverlap);

}
