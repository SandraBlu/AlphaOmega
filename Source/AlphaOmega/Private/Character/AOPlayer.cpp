// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AOPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InteractComponent.h"
#include "Components/AOInventoryComponent.h"
#include "EnhancedInputComponent.h"
#include "Items/AOEquipItem.h"
#include "Items/AOGearItem.h"
#include "Items/ThrowableItem.h"
#include "Framework/AOPlayerController.h"
#include "Items/AOPickup.h"
#include "Components/CapsuleComponent.h"
#include "Items/AOWeaponItem.h"
#include "Weapons/AOWeapon.h"
#include "Animation/AnimMontage.h"
#include "Weapons/ThrowableWeapon.h"

#define LOCTEXT_NAMESPACE "AOCharacter"

static FName NAME_ADSSocket("ADSSocket");

AAOPlayer::AAOPlayer()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCam = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCam"));
	FollowCam->SetupAttachment(CameraBoom);

	HeadAccessoryMesh = PlayerMeshes.Add(EEquipSlot::EIS_HeadAccessory, CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadAccessoryMesh")));
	TorsoMesh = PlayerMeshes.Add(EEquipSlot::EIS_Torso, CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TorsoMesh")));
	LegsMesh = PlayerMeshes.Add(EEquipSlot::EIS_Legs, CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegsMesh")));
	HandsMesh = PlayerMeshes.Add(EEquipSlot::EIS_Hands, CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandsMesh")));
	FeetMesh = PlayerMeshes.Add(EEquipSlot::EIS_Feet, CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FeetMesh")));
	BackpackMesh = PlayerMeshes.Add(EEquipSlot::EIS_Backpack, CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BackpackMesh")));
	Canteen = PlayerMeshes.Add(EEquipSlot::EIS_Canteen, CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Canteen")));

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

	bIsAiming = false;

	JogSpeed = 500.f;
	SprintSpeed = 800.f;

}

void AAOPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}
}

void AAOPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PerformInteractCheck();

// 	const float DesiredFOV = IsAiming() ? 70.f : 100.f;
// 	FollowCam->SetFieldOfView(FMath::FInterpTo(FollowCam->FieldOfView, DesiredFOV, DeltaTime, 10.f));
// 
// 	if (EquippedWeapon)
// 	{
// 		const FVector ADSLocation = EquippedWeapon->GetWeaponMesh()->GetSocketLocation(NAME_ADSSocket);
// 		const FVector DefaultCameraLocation = GetMesh()->GetSocketLocation(FName("CameraSocket"));
// 
// 		FVector CameraLoc = bIsAiming ? ADSLocation : DefaultCameraLocation;
// 
// 		const float InterpSpeed = FVector::Dist(ADSLocation, DefaultCameraLocation) / EquippedWeapon->ADSTime;
// 		FollowCam->SetWorldLocation(FMath::VInterpTo(FollowCam->GetComponentLocation(), CameraLoc, DeltaTime, InterpSpeed));
 //	}
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
			if (UInteractComponent* InteractComp = Cast<UInteractComponent>(TraceHit.GetActor()->GetComponentByClass(UInteractComponent::StaticClass())))
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

	if (UInteractComponent* Interactable = GetInteractable())
	{
		Interactable->EndFocus(this);
		if (InteractData.bInteractHeld)
		{
			CompleteInteract(false);
		}
	}
	InteractData.ViewedInteractComp = nullptr;
}

void AAOPlayer::NewInteractableFound(UInteractComponent* Interactable)
{
	UE_LOG(LogTemp, Warning, TEXT("Found an Interactable!!!"));
	CompleteInteract(false);

	if (UInteractComponent* OldInteractable = GetInteractable())
	{
		OldInteractable->EndFocus(this);
	}

	InteractData.ViewedInteractComp = Interactable;
	Interactable->BeginFocus(this);
}

void AAOPlayer::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if (UInteractComponent* Interactable = GetInteractable())
	{
		Interactable->Interact(this);
	}
}

void AAOPlayer::OnWeaponEquipped()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->OnEquip();
	}
}


void AAOPlayer::PlayTossFX(class UAnimMontage* TossMontage)
{
	PlayAnimMontage(TossMontage);
}

class UThrowableItem* AAOPlayer::GetThrowable() const
{
	UThrowableItem* EquippedThrowable = nullptr;

	if (EquippedItems.Contains(EEquipSlot::EIS_Throwable))
	{
		EquippedThrowable = Cast<UThrowableItem>(*EquippedItems.Find(EEquipSlot::EIS_Throwable));
	}
	return EquippedThrowable;
}

void AAOPlayer::SpawnThrowable()
{
	if (UThrowableItem* CurrentThrowable = GetThrowable())
	{
		if (CurrentThrowable->ThrowableClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = SpawnParams.Instigator = this;
			SpawnParams.bNoFail = true;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			FVector EyesLoc;
			FRotator EyesRot;
			GetController()->GetPlayerViewPoint(EyesLoc, EyesRot);
			//Move spawn point to slightly in front of player to avoid self collision
			EyesLoc = (EyesRot.Vector() * 20.f) + EyesLoc;

			if (AThrowableWeapon* ThrowableWeapon = GetWorld()->SpawnActor<AThrowableWeapon>(CurrentThrowable->ThrowableClass, FTransform(EyesRot, EyesLoc), SpawnParams))
			{
				PlayTossFX(CurrentThrowable->TossAnim);
			}

		}
	}
}

bool AAOPlayer::CanUseThrowable() const
{
	return GetThrowable() != nullptr && GetThrowable()->ThrowableClass != nullptr;
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
	OnEquippedItemsChanged.Broadcast(Item->Slot, Item);
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
				OnEquippedItemsChanged.Broadcast(Item->Slot, nullptr);
				return true;
			}
		}
	}
	return false;
}

void AAOPlayer::EquipGear(class UAOGearItem* Gear)
{
	if (USkeletalMeshComponent* GearMesh = *PlayerMeshes.Find(Gear->Slot))
	{
		GearMesh->SetSkeletalMeshAsset(Gear->Mesh);
		GearMesh->SetMaterial(GearMesh->GetMaterials().Num() - 1, Gear->MaterialInstance);

	}
}

void AAOPlayer::UnequipGear(const EEquipSlot Slot)
{
	if (USkeletalMeshComponent* EquipMesh = *PlayerMeshes.Find(Slot))
	{
		if (USkeletalMesh* BodyMesh = *NakedMeshes.Find(Slot))
		{
			EquipMesh->SetSkeletalMeshAsset(BodyMesh);
			//Set Material for new equip item

			for (int32 i = 0; i < BodyMesh->Materials.Num(); ++i)
			{
				if (BodyMesh->Materials.IsValidIndex(i))
				{
					EquipMesh->SetMaterial(i, BodyMesh->Materials[i].MaterialInterface);
				}
			}
		}
		else
		{
			//Logic for meshes without base mesh; ie backpack
			EquipMesh->SetSkeletalMeshAsset(nullptr);
		}
	}
}

void AAOPlayer::EquipWeapon(class UAOWeaponItem* WeaponItem)
{
	if (WeaponItem && WeaponItem->WeaponClass)
	{
		if (EquippedWeapon)
		{
			UnequipWeapon();
		}
		//Spawn in Weapon
		FActorSpawnParameters SpawnParams;
		SpawnParams.bNoFail = true;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParams.Owner = SpawnParams.Instigator = this;

		if (AAOWeapon* Weapon = GetWorld()->SpawnActor<AAOWeapon>(WeaponItem->WeaponClass, SpawnParams))
		{
			Weapon->Item = WeaponItem;

			EquippedWeapon = Weapon;
			OnWeaponEquipped();
			//Attach Weapon to Pawn
			Weapon->OnEquip();
		}
	}
}

void AAOPlayer::UnequipWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->OnUnEquip();
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;
		OnWeaponEquipped();
	}
}

bool AAOPlayer::CanAim() const
{
	return EquippedWeapon != nullptr;
}

void AAOPlayer::SetAiming(const bool bNewAiming)
{
	if ((bNewAiming && !CanAim()) || bNewAiming == bIsAiming)
	{
		return;
	}
	bIsAiming = bNewAiming;
}

class USkeletalMeshComponent* AAOPlayer::GetSlotSkeletalMeshComponent(const EEquipSlot Slot)
{
	if (PlayerMeshes.Contains(Slot))
	{
		return *PlayerMeshes.Find(Slot);
	}
	return nullptr;
}

void AAOPlayer::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AAOPlayer::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if (GetController())
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void AAOPlayer::CrouchStart(const FInputActionValue& Value)
{
	Crouch(true);	
}

void AAOPlayer::CrouchStop(const FInputActionValue& Value)
{	
	UnCrouch(true);
}

void AAOPlayer::StartInteract(const FInputActionValue& Value)
{
	InteractData.bInteractHeld = true;

	if (UInteractComponent* Interactable = GetInteractable())
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

	if (UInteractComponent* Interactable = GetInteractable())
	{
		Interactable->EndInteract(this);
	}
}

void AAOPlayer::DrawWeapon(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("weapon not valid"));
	if (IsValid(EquippedWeapon))
	{
		UE_LOG(LogTemp, Warning, TEXT("weapon valid"));
		FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
		EquippedWeapon->AttachToComponent(GetMesh(), TransformRules, "active_sword");
		EquippedWeapon->PlayEquipMontage(FName("DrawWeapon"));
	}
	
}

void AAOPlayer::UseThrowable(const FInputActionValue& Value)
{
	if (CanUseThrowable())
	{
		if (UThrowableItem* Throwable = GetThrowable())
		{
			PlayAnimMontage(Throwable->TossAnim);
			SpawnThrowable();

			if (PlayerInventory)
			{
				PlayerInventory->ConsumeItem(Throwable, 1);
			}
			else
			{
				if (Throwable->GetQuantity() <= 1)
				{
					EquippedItems.Remove(EEquipSlot::EIS_Throwable);
					OnEquippedItemsChanged.Broadcast(EEquipSlot::EIS_Throwable, nullptr);
				}
			}
		}		
	}
}

void AAOPlayer::StartAiming(const FInputActionValue& Value)
{
	if (CanAim())
	{
		SetAiming(true);
	}

}

void AAOPlayer::StopAiming(const FInputActionValue& Value)
{
	SetAiming(false);
}

void AAOPlayer::OpenMenu(const FInputActionValue& Value)
{
}

void AAOPlayer::OpenMap(const FInputActionValue& Value)
{
}

void AAOPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Movement
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAOPlayer::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAOPlayer::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AAOPlayer::CrouchStart);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AAOPlayer::CrouchStop);

		//Interact
		EnhancedInputComponent->BindAction(InterAction, ETriggerEvent::Started, this, &AAOPlayer::StartInteract);
		EnhancedInputComponent->BindAction(InterAction, ETriggerEvent::Completed, this, &AAOPlayer::CompleteInteract);

		//UI
		EnhancedInputComponent->BindAction(OpenMenuAction, ETriggerEvent::Started, this, &AAOPlayer::OpenMenu);
		EnhancedInputComponent->BindAction(OpenMapAction, ETriggerEvent::Started, this, &AAOPlayer::OpenMenu);

		//Weapon
		EnhancedInputComponent->BindAction(DrawWeaponAction, ETriggerEvent::Started, this, &AAOPlayer::DrawWeapon);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Started, this, &AAOPlayer::UseThrowable);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AAOPlayer::StartAiming);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AAOPlayer::StopAiming);
	}
	
}

#undef LOCTEXT_NAMESPACE