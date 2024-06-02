// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AOPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Framework/AOPlayerController.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimMontage.h"
#include "Framework/AOPlayerState.h"
#include <UI/AOHUD.h>
#include <AbilitySystem/AOAbilityComp.h>
#include "AbilitySystem/Data/XPInfo.h"
#include "NiagaraComponent.h"
#include <Components/AOInputComponent.h>
#include "AOGameplayTags.h"
#include "Components/AOEquipmentComponent.h"
#include "Weapons/AOWeapon.h"


#define LOCTEXT_NAMESPACE "AOCharacter"

AAOPlayer::AAOPlayer()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCam = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCam"));
	FollowCam->SetupAttachment(CameraBoom);

	CharacterClass = ECharacterClass::Elementalist;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;

	LevelUpFX = CreateDefaultSubobject<UNiagaraComponent>("LevelUpComp");
	LevelUpFX->SetupAttachment(GetRootComponent());
	LevelUpFX->bAutoActivate = false;

	Gear = CreateDefaultSubobject<UAOEquipmentComponent>("GearComp");
}

void AAOPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	InitAbilityActorInfo();
	GrantAbilities();
}

int32 AAOPlayer::GetAbilityPoints_Implementation() const
{
	AAOPlayerState* AOPlayerState = GetPlayerState<AAOPlayerState>();
	check(AOPlayerState)
	return 	AOPlayerState->GetAbilityPts();
}

int32 AAOPlayer::GetAttributePoints_Implementation() const
{
	AAOPlayerState* AOPlayerState = GetPlayerState<AAOPlayerState>();
	check(AOPlayerState)
	return 	AOPlayerState->GetAttributePts();
}

void AAOPlayer::AddToAbilityPoints_Implementation(int32 InAbilityPoints)
{
	AAOPlayerState* AOPlayerState = GetPlayerState<AAOPlayerState>();
	check(AOPlayerState)
	return 	AOPlayerState->AddToAbilityPts(InAbilityPoints);
}

void AAOPlayer::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	AAOPlayerState* AOPlayerState = GetPlayerState<AAOPlayerState>();
	check(AOPlayerState)
	return 	AOPlayerState->AddToAttributePts(InAttributePoints);
}

void AAOPlayer::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	AAOPlayerState* AOPlayerState = GetPlayerState<AAOPlayerState>();
	check(AOPlayerState)
	return 	AOPlayerState->AddToLevel(InPlayerLevel);
}

int32 AAOPlayer::GetAbilityPointsReward_Implementation(int32 Level) const
{
	AAOPlayerState* AOPlayerState = GetPlayerState<AAOPlayerState>();
	check(AOPlayerState)
	return 	AOPlayerState->XPInfo->LevelUpInfo[Level].AbilityPointsRewarded;
}

int32 AAOPlayer::GetAttributePointsReward_Implementation(int32 Level) const
{
	AAOPlayerState* AOPlayerState = GetPlayerState<AAOPlayerState>();
	check(AOPlayerState)
		return 	AOPlayerState->XPInfo->LevelUpInfo[Level].AttributePointsRewarded;
}

int32 AAOPlayer::FindLevelForXP_Implementation(int32 InXP) const
{
	AAOPlayerState* AOPlayerState = GetPlayerState<AAOPlayerState>();
	check(AOPlayerState)
	return 	AOPlayerState->XPInfo->FindLevelForXP(InXP);
}

int32 AAOPlayer::GetXP_Implementation() const
{
	AAOPlayerState* AOPlayerState = GetPlayerState<AAOPlayerState>();
	check(AOPlayerState)
	return 	AOPlayerState->GetXP();
}

void AAOPlayer::LevelUp_Implementation()
{
	if (IsValid(LevelUpFX))
	{
		LevelUpFX->Activate(true);
		if (GetASC() == nullptr) return;
		AAOPlayerState* AOPlayerState = GetPlayerState<AAOPlayerState>();
		check(AOPlayerState)
		
		GetASC()->UpdateAbilityStatus(AOPlayerState->GetCharacterLevel());
		//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, *InputTag.ToString());
	}
}

void AAOPlayer::AddToXP_Implementation(int32 InXP)
{
	AAOPlayerState* AOPlayerState = GetPlayerState<AAOPlayerState>();
	check(AOPlayerState)
	return AOPlayerState->AddToXP(InXP);
}

void AAOPlayer::InitAbilityActorInfo()
{
	AAOPlayerState* AOPlayerState = GetPlayerState<AAOPlayerState>();
	check(AOPlayerState)
	AOPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AOPlayerState, this);
	Cast<UAOAbilityComp>(AOPlayerState->GetAbilitySystemComponent())->AbilityActorInfoInit();
	AbilitySystemComponent = AOPlayerState->GetAbilitySystemComponent();
	AttributeSet = AOPlayerState->GetAttributeSet();
	if (AAOPlayerController* AOPC = Cast<AAOPlayerController>(GetController()))
	{
		if (AAOHUD* Hud = Cast<AAOHUD>(AOPC->GetHUD()))
		{
			Hud->InitOverlay(AOPC, AOPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
	InitializeAttributes();
}

int32 AAOPlayer::GetCharacterLevel_Implementation()
{
	const AAOPlayerState* AOPlayerState = GetPlayerState<AAOPlayerState>();
	check(AOPlayerState)
	return AOPlayerState->GetCharacterLevel();
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

void AAOPlayer::SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled)
{
	if (Gear->EquippedWeapon && Gear->EquippedWeapon->GetWeaponBox())
	{
		Gear->EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		Gear->EquippedWeapon->IgnoreActors.Empty();
	}
}

FVector AAOPlayer::GetCombatSocketLocation_Implementation(const FGameplayTag& CombatSocketTag)
{
	const FAOGameplayTags& GameplayTags = FAOGameplayTags::Get();
	if (CombatSocketTag.MatchesTagExact(GameplayTags.combatSocket_weapon) && IsValid(Gear->EquippedWeapon))
	{
		return Gear->EquippedWeapon->GetWeaponMesh()->GetSocketLocation(Gear->EquippedWeapon->FiringSocket);
	}
	if (CombatSocketTag.MatchesTagExact(GameplayTags.combatSocket_handL))
	{
		return GetMesh()->GetSocketLocation(LHand);
	}
	if (CombatSocketTag.MatchesTagExact(GameplayTags.combatSocket_handR))
	{
		return GetMesh()->GetSocketLocation(RHand);
	}
	return FVector();
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

void AAOPlayer::DrawWeapon(const FInputActionValue& Value)
{
	if (CanDisarm())
	{
		Gear->EquippedWeapon->PlayEquipMontage(FName("Disarm"));
		EquipState = EEquipState::EES_Unequipped;
		ActionState = EActionState::EAS_EquippingWeapon;
	}
	else if (CanDraw())
	{
		Gear->EquippedWeapon->PlayEquipMontage(FName("Draw"));
		EquipState = EEquipState::EES_EquippedMelee;
		ActionState = EActionState::EAS_EquippingWeapon;
	}	
}

bool AAOPlayer::CanDisarm()
{
	return ActionState == EActionState::EAS_Idle && EquipState != EEquipState::EES_Unequipped;
}

bool AAOPlayer::CanDraw()
{
	return ActionState == EActionState::EAS_Idle && EquipState == EEquipState::EES_Unequipped && EquippedWeapon;
}

void AAOPlayer::Disarm()
{
	if (Gear->EquippedWeapon)
	{
		Gear->EquippedWeapon->OnUnEquip();
	}
}

void AAOPlayer::Draw()
{
	if (Gear->EquippedWeapon)
	{
		Gear->EquippedWeapon->OnEquip();
	}
}

void AAOPlayer::FinishEquipping()
{
	ActionState = EActionState::EAS_Idle;
}

bool AAOPlayer::CanAttack()
{
	return ActionState == EActionState::EAS_Idle &&
		EquipState != EEquipState::EES_Unequipped;
}

void AAOPlayer::AttackEnd()
{
	ActionState = EActionState::EAS_Idle;
}

void AAOPlayer::PrimaryAttack(const FInputActionValue& Value)
{
	if (CanAttack())
	{
		ActionState = EActionState::EAS_Attacking;
	}
}

//void AAOPlayer::UseThrowable(const FInputActionValue& Value)
//{
//	/*if (CanUseThrowable())
//	{
//		if (UThrowableItem* Throwable = GetThrowable())
//		{
//			PlayAnimMontage(Throwable->TossAnim);
//			SpawnThrowable();
//
//			if (PlayerInventory)
//			{
//				PlayerInventory->ConsumeItem(Throwable, 1);
//			}
//			else
//			{
//				if (Throwable->GetQuantity() <= 1)
//				{
//					EquippedItems.Remove(EEquipSlot::EIS_Throwable);
//					OnEquippedItemsChanged.Broadcast(EEquipSlot::EIS_Throwable, nullptr);
//				}
//			}
//		}
//	}*/
//}

void AAOPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UAOInputComponent* InputComp = CastChecked<UAOInputComponent>(InputComponent))
	{
		//Movement
		InputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAOPlayer::Move);
		InputComp->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAOPlayer::Look);
		
		//Weapon
		InputComp->BindAction(DrawWeaponAction, ETriggerEvent::Started, this, &AAOPlayer::DrawWeapon);
		InputComp->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AAOPlayer::PrimaryAttack);
		
		InputComp->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
	}
	
}

void AAOPlayer::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, *InputTag.ToString());
}

void AAOPlayer::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC() == nullptr) return;
	GetASC()->AbilityInputTagReleased(InputTag);
}

void AAOPlayer::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetASC() == nullptr) return;
	GetASC()->AbilityInputTagHeld(InputTag);
}

#undef LOCTEXT_NAMESPACE