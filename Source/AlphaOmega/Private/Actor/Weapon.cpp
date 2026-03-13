// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon.h"

// Sets default values
AWeapon::AWeapon()
{
	SKMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SKMesh->bReceivesDecals = false;
	SKMesh->SetCollisionObjectType(ECC_WorldDynamic);
	SKMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SKMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = SKMesh;
	InactiveWeaponSocket = FName("inactive_katana");
	ActiveWeaponSocket = FName("weapon_r");

}

void AWeapon::OnEquip()
{
	//AttachMeshToPawn(PawnOwner->GetMesh(), ActiveWeaponSocket);
	PlayEquipMontage("Draw");
	IsEquipped = true;
}

void AWeapon::OnUnEquip()
{
	//AttachMeshToPawn(PawnOwner->GetMesh(), InactiveWeaponSocket);
	IsEquipped = false;
}

USkeletalMeshComponent* AWeapon::GetWeaponMesh() const
{
	return SKMesh;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	//PawnOwner = Cast<ARPlayer>(GetOwner());
}

void AWeapon::PlayEquipMontage(const FName SectionName)
{
	//UAnimInstance* AnimInstance = PawnOwner->GetMesh()->GetAnimInstance();
//	if (AnimInstance && EquipAnim)
//	{
//		AnimInstance->Montage_Play(EquipAnim);
//		AnimInstance->Montage_JumpToSection(SectionName, EquipAnim);
//	}
}

/*class ARPlayer* AWeapon::GetPawnOwner() const
{
	return PawnOwner;
}

void AWeapon::AttachMeshToPawn(USceneComponent* InParent, FName InSocketName)
{
	if (PawnOwner)
	{
		FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
		USkeletalMeshComponent* PawnMesh = PawnOwner->GetMesh();
		AttachToComponent(PawnOwner->GetMesh(), TransformRules, InSocketName);
	}
}*/

