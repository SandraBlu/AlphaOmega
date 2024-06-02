// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/AOEquipItem.h"
#include "Components/AOEquipmentComponent.h"
#include "Engine/SkinnedAssetCommon.h"
#include "Weapons/AOWeapon.h"

#define LOCTEXT_NAMESPACE "EquippableItem"

UAOEquipItem::UAOEquipItem()
{
	bStackable = false;
	Weight = 1.f;
	bCanActivate = true;
	bToggleActiveOnUse = true;
	UseActionText = LOCTEXT("ItemUseActionText", "Equip");
}

void UAOEquipItem::Deactivated_Implementation()
{
	if (GetOwningPawn())
	{
		UseActionText = LOCTEXT("EquipText", "Equip");

		if (UAOEquipmentComponent* EquipmentComponent = Cast<UAOEquipmentComponent>(GetOwningPawn()->GetComponentByClass(UAOEquipmentComponent::StaticClass())))
		{
			EquipmentComponent->UnequipItem(this);
		}
	}
}

void UAOEquipItem::Activated_Implementation()
{
	if (GetOwningPawn())
	{
		UseActionText = LOCTEXT("UnequipText", "Unequip");

		if (UAOEquipmentComponent* EquipmentComponent = Cast<UAOEquipmentComponent>(GetOwningPawn()->GetComponentByClass(UAOEquipmentComponent::StaticClass())))
		{
			EquipmentComponent->EquipItem(this);
		}
	}
}

void UAOEquipItem::HandleEquip_Implementation()
{
}

void UAOEquipItem::HandleUnequip_Implementation()
{
}

UAOEquipItem_Clothing::UAOEquipItem_Clothing()
{
	
}
void UAOEquipItem_Clothing::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UAOEquipItem_Clothing, ClothingMesh))
	{
		if (ClothingMesh)
		{
			ClothingMaterials.Empty();

			for (auto& ClothingMeshMat : ClothingMesh->GetMaterials())
			{
				ClothingMaterials.Add(ClothingMeshMat.MaterialInterface);
			}
		}
	}
}
void UAOEquipItem_Clothing::HandleUnequip_Implementation()
{
	if (UAOEquipmentComponent* EquipmentComponent = Cast<UAOEquipmentComponent>(GetOwningPawn()->GetComponentByClass(UAOEquipmentComponent::StaticClass())))
	{
		if (EquipmentComponent->EquippableComponents.Contains(Slot) && EquipmentComponent->DefaultClothing.Contains(Slot) && EquipmentComponent->DefaultClothingMaterials.Contains(Slot))
		{
			//Set the clothing back to its default mesh and materials
			if (USkeletalMeshComponent* SlotComponent = *EquipmentComponent->EquippableComponents.Find(Slot))
			{
				if (USkeletalMesh* DefaultClothingMesh = *EquipmentComponent->DefaultClothing.Find(Slot))
				{
					SlotComponent->SetSkeletalMesh(DefaultClothingMesh);

					if (FDefaultClothingMeshMaterials* DefaultMaterials = EquipmentComponent->DefaultClothingMaterials.Find(Slot))
					{
						int32 Idx = 0;

						for (auto& DefaultMat : DefaultMaterials->Materials)
						{
							SlotComponent->SetMaterial(Idx, DefaultMat);
							++Idx;
						}
					}
				}
				else 
				{
					SlotComponent->SetSkeletalMesh(nullptr);
				}

				if(EquipmentComponent->LeaderPoseComponent)
				{
					SlotComponent->SetLeaderPoseComponent(EquipmentComponent->LeaderPoseComponent);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Narrative Equipment tried unequipping item but the Equipment Component doesn't have a skeletal mesh component added. Have you called initialize?"), *GetNameSafe(this));
		}

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Narrative Equipment tried unequipping your item, but your pawn doesn't have an Equipment Component added. Please add one."));
	}
}

void UAOEquipItem_Clothing::HandleEquip_Implementation()
{
	if (UAOEquipmentComponent* EquipmentComponent = Cast<UAOEquipmentComponent>(GetOwningPawn()->GetComponentByClass(UAOEquipmentComponent::StaticClass())))
	{
		if (ClothingMesh)
		{
			if (EquipmentComponent->EquippableComponents.Contains(Slot))
			{
				//Set the clothing mesh to the new mesh
				if (USkeletalMeshComponent* SlotComponent = *EquipmentComponent->EquippableComponents.Find(Slot))
				{
					SlotComponent->SetSkeletalMesh(ClothingMesh);

					int32 Idx = 0;
					for (auto& Mat : ClothingMaterials)
					{
						SlotComponent->SetMaterial(Idx, Mat);
						++Idx;
					}

					if (EquipmentComponent->LeaderPoseComponent)
					{
						SlotComponent->SetLeaderPoseComponent(EquipmentComponent->LeaderPoseComponent);
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Narrative Equipment tried equipping %s but the Equipment Component doesn't have a skeletal mesh component added. Have you called Initialize?"), *GetNameSafe(this));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Narrative Equipment tried putting an item on, but your pawn doesn't have an Equipment Component added. Please add one."));
	}
}

UAOEquipItem_Weapon::UAOEquipItem_Weapon()
{
}

void UAOEquipItem_Weapon::HandleEquip_Implementation()
{
	if (WeaponClass)
	{
		UAOEquipmentComponent* EquipmentComponent = Cast<UAOEquipmentComponent>(GetOwningPawn()->GetComponentByClass(UAOEquipmentComponent::StaticClass()));
		if (EquipmentComponent->EquippedWeapon)
		{
			EquipmentComponent->EquippedWeapon->OnUnEquip();
			EquipmentComponent->EquippedWeapon->Destroy();
			EquipmentComponent->EquippedWeapon = nullptr;
			EquipmentComponent->OnWeaponEquipped();
		}
		//Spawn in Weapon
		FActorSpawnParameters SpawnParams;
		SpawnParams.bNoFail = true;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParams.Owner = SpawnParams.Instigator = GetOwningPawn();

		if (AAOWeapon* Weapon = GetWorld()->SpawnActor<AAOWeapon>(WeaponClass, SpawnParams))
		{
			EquipmentComponent->EquippedWeapon = Weapon;
			EquipmentComponent->OnWeaponEquipped();
			//Attach Weapon to Pawn
			Weapon->OnEquip();
		}
	}
}

void UAOEquipItem_Weapon::HandleUnequip_Implementation()
{
	UAOEquipmentComponent* EquipmentComponent = Cast<UAOEquipmentComponent>(GetOwningPawn()->GetComponentByClass(UAOEquipmentComponent::StaticClass()));
	if (EquipmentComponent->EquippedWeapon)
	{
		EquipmentComponent->EquippedWeapon->OnUnEquip();
		EquipmentComponent->EquippedWeapon->Destroy();
		EquipmentComponent->EquippedWeapon = nullptr;
		EquipmentComponent->OnWeaponEquipped();
	}
}
#undef LOCTEXT_NAMESPACE
