// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/AOWeapon.h"
#include "Character/AOPlayer.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NiagaraComponent.h"
#include "Framework/AOPlayerController.h"
#include "GameFramework/DamageType.h"

// Sets default values
AAOWeapon::AAOWeapon()
{
    SKMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    SKMesh->bReceivesDecals = false;
    SKMesh->SetCollisionObjectType(ECC_WorldDynamic);
    SKMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SKMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    RootComponent = SKMesh;

    WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
    WeaponBox->SetupAttachment(GetRootComponent());

    bIsAttacking = false;
    bIsEquipped = false;
    bPendingEquip = false;
    CurrentState = EWeaponState::Idle;
    CurrentType = EWeaponType::Melee;
    InactiveWeaponSocket = FName("inactive_sword");
    ActiveWeaponSocket = FName("active_sword");

}

// Called when the game starts or when spawned
void AAOWeapon::BeginPlay()
{
	Super::BeginPlay();

    PawnOwner = Cast<AAOPlayer>(GetOwner());
   // WeaponHitZone->OnComponentBeginOverlap.AddDynamic(this, &AAOWeapon::OnBoxOverlap);
}

void AAOWeapon::OnEquip()
{
    AttachMeshToPawn(PawnOwner->GetMesh(), ActiveWeaponSocket);
    bIsEquipped = false;
    bPendingEquip = true;
    if (CanDisarm())
    {
        PlayEquipMontage(FName("Disarm"));
        CurrentType = EWeaponType::Unarmed;
    }
    else if (CanDrawWeapon())
    {
        PlayEquipMontage(FName("DrawWeapon"));
        CurrentState = EWeaponState::Equipping;
    }
    DetermineWeaponState();
   
    OnEquipFinished();
  
}

void AAOWeapon::OnEquipFinished()
{
    AttachMeshToPawn(PawnOwner->GetMesh(), ActiveWeaponSocket);

    bIsEquipped = true;
    bPendingEquip = false;
   
   DetermineWeaponState();
   SetWeaponType(CurrentType);
}

void AAOWeapon::OnUnEquip()
{
    bIsEquipped = false;
    if (bPendingEquip)
    {
        bPendingEquip = false;
        CurrentType = EWeaponType::Unarmed;
    }
    DetermineWeaponState();
}

bool AAOWeapon::IsAttacking() const
{
    SKMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    return bIsAttacking &&
        CurrentState != EWeaponState::Attacking;
}

bool AAOWeapon::IsEquipped() const
{
    return bIsEquipped &&
        CurrentType != EWeaponType::Unarmed;
}

bool AAOWeapon::IsAttachedToPawn() const
{
    return bIsEquipped || bPendingEquip;
}

bool AAOWeapon::CanAttack() const
{
    bool bCanAttack = PawnOwner != nullptr;
    bool bStateOKToAttack = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Attacking));
    return ((bCanAttack == true) && (bStateOKToAttack == true) && (bIsEquipped == true));
}

EWeaponState AAOWeapon::GetCurrentState() const
{
    return CurrentState;
}

EWeaponType AAOWeapon::GetCurrentType() const
{
    return CurrentType;
}

USkeletalMeshComponent* AAOWeapon::GetWeaponMesh() const
{
    return SKMesh;
}

class AAOPlayer* AAOWeapon::GetPawnOwner() const
{
    return PawnOwner;
}

void AAOWeapon::SetWeaponState(EWeaponState NewState)
{
    const EWeaponState PrevState = CurrentState;

    if (PrevState == EWeaponState::Attacking && NewState != EWeaponState::Attacking)
    {
        DetermineWeaponState();
    }

    CurrentState = NewState;

    if (PrevState != EWeaponState::Attacking && NewState == EWeaponState::Attacking)
    {
        DetermineWeaponState();
    }
}

void AAOWeapon::SetWeaponType(EWeaponType NewType)
{
    const EWeaponType PrevType = CurrentType;

    if (PrevType == EWeaponType::Melee && NewType != EWeaponType::Melee)
    {
        NewType = EWeaponType::Ranged;
    }
    CurrentType = NewType;

    if (PrevType != EWeaponType::Melee && NewType == EWeaponType::Melee)
    {
        NewType = EWeaponType::Melee;
    }
}

void AAOWeapon::DetermineWeaponState()
{
    EWeaponState NewState = EWeaponState::Idle;

    if (bIsEquipped)
    { 
       if ((bIsAttacking == true) && (CanAttack() == true))
        {
           NewState = EWeaponState::Attacking;
        }
    }
    else if (bPendingEquip)
    {
        NewState = EWeaponState::Equipping;
    }
    SetWeaponState(NewState);
}

void AAOWeapon::AttachMeshToPawn(USceneComponent* InParent, FName InSocketName)
{
    if (PawnOwner)
    {
        FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
        USkeletalMeshComponent* PawnMesh = PawnOwner->GetMesh();
        AttachToComponent(PawnOwner->GetMesh(), TransformRules, InSocketName);
    }
}

 void AAOWeapon::Draw()
 {
     AttachMeshToPawn(PawnOwner->GetMesh(), InactiveWeaponSocket);
 }
 
 void AAOWeapon::Disarm()
 {
     AttachMeshToPawn(PawnOwner->GetMesh(), ActiveWeaponSocket);
 }

 void AAOWeapon::PlayEquipMontage(FName SectionName)
 {
     UAnimInstance* AnimInstance = PawnOwner->GetMesh()->GetAnimInstance();
     if (AnimInstance && EquipAnim)
     {
         AnimInstance->Montage_Play(EquipAnim);
         AnimInstance->Montage_JumpToSection(SectionName, EquipAnim);
     }
 }
 
 void AAOWeapon::FinishDraw()
 {
     CurrentState = EWeaponState::Idle;
 }
 
 bool AAOWeapon::CanDisarm()
 {
     return CurrentState == EWeaponState::Idle && (bIsEquipped == true);
 }

 bool AAOWeapon::CanDrawWeapon()
 {
     return CurrentState == EWeaponState::Idle && (bIsEquipped == false);
 }