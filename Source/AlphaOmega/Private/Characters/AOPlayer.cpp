// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AOPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
//#include "EnhancedInputSubsystems.h"
//#include "EnhancedInputComponent.h"
#include "Items/AOEquipItem.h"
#include "Components/AOInteractComponent.h"
#include "Components/AOInventoryComponent.h"

#define LOCTEXT_NAMESPACE "AOCharacter"

AAOPlayer::AAOPlayer()
{
	FollowTPC = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCam"));
	FollowTPC->SetupAttachment(TPCCamBoom);

	HeadAccessoryMesh = PlayerMeshes.Add(EEquipSlot::EIS_HeadAccessory, CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadAccessoryMesh")));
	TorsoMesh = PlayerMeshes.Add(EEquipSlot::EIS_Torso, CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TorsoMesh")));
	LegsMesh = PlayerMeshes.Add(EEquipSlot::EIS_Legs, CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegsMesh")));
	HandsMesh = PlayerMeshes.Add(EEquipSlot::EIS_Hands, CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandsMesh")));
	FeetMesh = PlayerMeshes.Add(EEquipSlot::EIS_Feet, CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FeetMesh")));
	BackpackMesh = PlayerMeshes.Add(EEquipSlot::EIS_Backpack, CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BackpackMesh")));

	//Leader Pose Component
	for (auto& PlayerMesh : PlayerMeshes)
	{
		USkeletalMeshComponent* MeshComp = PlayerMesh.Value;
		MeshComp->SetupAttachment(GetMesh());
		MeshComp->SetLeaderPoseComponent(GetMesh());
	}
	//Add Head to Player mesh after Leader pose function so it does not attach self to self

	PlayerMeshes.Add(EEquipSlot::EIS_Head, GetMesh());

	//Interact
	InteractCheckFrequency = 0.f;
	TraceDistance = 1000.f;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
}

void AAOPlayer::PlayEquipMontage(const FName SectionName)
{

}

void AAOPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		/*if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}*/
	}
}

void AAOPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PerformInteractCheck();
}

void AAOPlayer::PerformInteractCheck()
{
	if (GetController() == nullptr)
	{
		return;
	}

	InteractData.LastInteractCheckTime = GetWorld()->GetTimeSeconds();

	FVector EyeLoc;
	FRotator EyeRot;
	GetController()->GetPlayerViewPoint(EyeLoc, EyeRot);

	FVector TraceStart = EyeLoc;
	FVector TraceEnd = (EyeRot.Vector() * TraceDistance) + TraceStart;
	TArray<FHitResult> TraceHits;

	FHitResult TraceHit;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{

		if (TraceHit.GetActor())
		{
			DrawDebugSphere(GetWorld(), TraceHit.ImpactPoint, 10.f, 32, FColor::Red, false, 0.0f);
			if (UAOInteractComponent* InteractComp = Cast<UAOInteractComponent>(TraceHit.GetActor()->GetComponentByClass(UAOInteractComponent::StaticClass())))
			{
				float Distance = (TraceStart - TraceHit.ImpactPoint).Size();
				if (InteractComp != GetInteractable() && Distance <= InteractComp->InteractDistance)
				{
					NewInteractableFound(InteractComp);
				}
				else if (Distance > InteractComp->InteractDistance && GetInteractable())
				{
					NoInteractableFound();
				}
				return;
			}
		}
	}
	NoInteractableFound();
}

void AAOPlayer::NoInteractableFound()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_Interact))
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interact);
	}

	if (UAOInteractComponent* Interactable = GetInteractable())
	{
		Interactable->EndFocus(this);
		if (InteractData.bInteractHeld)
		{
			//CompleteInteract(false);
		}
	}
	InteractData.ViewedInteractComp = nullptr;
}

void AAOPlayer::NewInteractableFound(UAOInteractComponent* Interactable)
{
	UE_LOG(LogTemp, Warning, TEXT("Found an Interactable!!!"));
	//CompleteInteract(false);

	if (UAOInteractComponent* OldInteractable = GetInteractable())
	{
		OldInteractable->EndFocus(this);
	}

	InteractData.ViewedInteractComp = Interactable;
	Interactable->BeginFocus(this);
}

void AAOPlayer::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if (UAOInteractComponent* Interactable = GetInteractable())
	{
		Interactable->Interact(this);
	}
}

bool AAOPlayer::IsInteracting() const
{
	return GetWorldTimerManager().IsTimerActive(TimerHandle_Interact);
}

float AAOPlayer::GetRemainingInteractTime() const
{
	return GetWorldTimerManager().GetTimerRemaining(TimerHandle_Interact);
}

void AAOPlayer::UseItem(class UAOItem* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("no item"));
	if (PlayerInventory && !PlayerInventory->FindItem(Item))
	{
		return;
	}

	if (Item)
	{
		Item->OnUse(this);
		Item->Use(this);
	}
}

void AAOPlayer::DropItem(class UAOItem* Item, const int32 Quantity)
{
	if (PlayerInventory && Item && PlayerInventory->FindItem(Item))
	{
		const int32 ItemQuantity = Item->GetQuantity();
		const int32 DroppedQuantity = PlayerInventory->ConsumeItem(Item, Quantity);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.bNoFail = true;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		FVector SpawnLocation = GetActorLocation();
		SpawnLocation.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		FTransform SpawnTransform(GetActorRotation(), SpawnLocation);

		ensure(PickupClass);

		AAOPickup* Pickup = GetWorld()->SpawnActor<AAOPickup>(PickupClass, SpawnTransform, SpawnParams);
		Pickup->InitializePickup(Item->GetClass(), DroppedQuantity);
	}
}

bool AAOPlayer::EquipItem(class UAOEquipItem* Item)
{
	EquippedItems.Add(Item->Slot, Item);
	//OnEquippedItemsChanged.Broadcast(Item->Slot, Item);
	return true;
}

bool AAOPlayer::UnequipItem(class UAOEquipItem* Item)
{
	if (Item)
	{
		if (EquippedItems.Contains(Item->Slot))
		{
			if (Item == *EquippedItems.Find(Item->Slot))
			{
				EquippedItems.Remove(Item->Slot);
				//OnEquippedItemsChanged.Broadcast(Item->Slot, nullptr);
				return true;
			}
		}
	}
	return false;
}

void AAOPlayer::StartInteract(const FInputActionValue& Value)
{
	InteractData.bInteractHeld = true;

	if (UAOInteractComponent* Interactable = GetInteractable())
	{
		Interactable->BeginInteract(this);
		if (FMath::IsNearlyZero(Interactable->InteractTime))
		{
			Interact();
		}
		else
		{
			GetWorldTimerManager().SetTimer(TimerHandle_Interact, this, &AAOPlayer::Interact, Interactable->InteractTime, false);
		}
	}
}

void AAOPlayer::CompleteInteract(const FInputActionValue& Value)
{
	InteractData.bInteractHeld = false;

	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if (UAOInteractComponent* Interactable = GetInteractable())
	{
		Interactable->EndInteract(this);
	}
}

void AAOPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{

		//Interact
		//EnhancedInputComponent->BindAction(InterAction, ETriggerEvent::Started, this, &AAOPlayer::StartInteract);
		//EnhancedInputComponent->BindAction(InterAction, ETriggerEvent::Completed, this, &AAOPlayer::CompleteInteract);

		//Weapon
		//EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Started, this, &AAOPlayer::UseThrowable);

	}
}

#undef LOCTEXT_NAMESPACE
