// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/EquipItem.h"

#include "Actor/Weapon.h"
#include "Inventory/EquipmentComponent.h"

#define LOCTEXT_NAMESPACE "EquipItem"

UEquipItem::UEquipItem()
{
	bStackable = false;
	Weight = 1.f;
	bCanActivate = true;
	bToggleActiveOnUse = true;
	UseActionText = LOCTEXT("ItemUseActionText", "Equip");
}

void UEquipItem::HandleEquip_Implementation()
{

}

void UEquipItem::HandleUnequip_Implementation()
{

}

void UEquipItem::Activated_Implementation()
{
	if (GetOwningPawn())
	{
		UseActionText = LOCTEXT("UnequipText", "Unequip");

		if (UEquipmentComponent* EquipmentComponent = Cast<UEquipmentComponent>(GetOwningPawn()->GetComponentByClass(UEquipmentComponent::StaticClass())))
		{
			EquipmentComponent->EquipItem(this);
		}
	}
}

void UEquipItem::Deactivated_Implementation()
{
	if (GetOwningPawn())
	{
		UseActionText = LOCTEXT("EquipText", "Equip");

		if (UEquipmentComponent* EquipmentComponent = Cast<UEquipmentComponent>(GetOwningPawn()->GetComponentByClass(UEquipmentComponent::StaticClass())))
		{
			EquipmentComponent->UnequipItem(this);
		}
	}
}

UEquipItem_Clothing::UEquipItem_Clothing()
{

}

void UEquipItem_Clothing::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEquipItem_Clothing, ClothingMesh))
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

void UEquipItem_Clothing::HandleUnequip_Implementation()
{
	if (UEquipmentComponent* EquipmentComponent = Cast<UEquipmentComponent>(GetOwningPawn()->GetComponentByClass(UEquipmentComponent::StaticClass())))
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

				if (EquipmentComponent->LeaderPoseComponent)
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

void UEquipItem_Clothing::HandleEquip_Implementation()
{
	if (UEquipmentComponent* EquipmentComponent = Cast<UEquipmentComponent>(GetOwningPawn()->GetComponentByClass(UEquipmentComponent::StaticClass())))
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

UEquipItem_Weapon::UEquipItem_Weapon()
{

}

void UEquipItem_Weapon::HandleUnequip_Implementation()
{
	UEquipmentComponent* EquipmentComponent = Cast<UEquipmentComponent>(GetOwningPawn()->GetComponentByClass(UEquipmentComponent::StaticClass()));
	if (EquipmentComponent->EquippedWeapon)
	{
		EquipmentComponent->EquippedWeapon->OnUnEquip();
		EquipmentComponent->EquippedWeapon->Destroy();
		EquipmentComponent->EquippedWeapon = nullptr;
		EquipmentComponent->OnWeaponEquipped();
	}
}

void UEquipItem_Weapon::HandleEquip_Implementation()
{
	if (WeaponClass)
	{
		UEquipmentComponent* EquipmentComponent = Cast<UEquipmentComponent>(GetOwningPawn()->GetComponentByClass(UEquipmentComponent::StaticClass()));
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

		if (AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, SpawnParams))
		{
			EquipmentComponent->EquippedWeapon = Weapon;
			EquipmentComponent->OnWeaponEquipped();
			//Attach Weapon to Pawn
			Weapon->OnEquip();
		}
	}
}

#undef LOCTEXT_NAMESPACE