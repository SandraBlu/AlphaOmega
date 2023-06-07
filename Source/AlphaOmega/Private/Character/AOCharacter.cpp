// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AOCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InteractComponent.h"
#include "Components/AOCoreComponent.h"
#include "Framework/AOBFL.h"
#include "Attributes/AOPlayerAttributeSet.h"
#include "Framework/AOPlayerController.h"
#include "Items/AOItem.h"
#include "Components/AOInventoryComponent.h"



// Sets default values
AAOCharacter::AAOCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);

	AbilitySystemComp = CreateDefaultSubobject<UAbilitySystemComponent>("AbilityComp");
	Attributes = CreateDefaultSubobject<UAOPlayerAttributeSet>("Attributes");

	PlayerInventory = CreateDefaultSubobject<UAOInventoryComponent>("InventoryComp");
	PlayerInventory->SetCapacity(20);
	PlayerInventory->SetWeightCapacity(80.f);
}

UAbilitySystemComponent* AAOCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComp;
}

void AAOCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (AbilitySystemComp)
		AbilitySystemComp->InitAbilityActorInfo(this, this);

	InitializeAttributes();
	GrantAbilities();
}

void AAOCharacter::InitializeAttributes()
{
	if (AbilitySystemComp && GrantedEffect)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComp->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComp->MakeOutgoingSpec(GrantedEffect, 1, EffectContext);

		if (SpecHandle.IsValid())
			FActiveGameplayEffectHandle GEHandle = AbilitySystemComp->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AAOCharacter::GrantAbilities()
{
	if (AbilitySystemComp)
		for (TSubclassOf<UGameplayAbility>& StartupAbility : GrantedAbilities)
			AbilitySystemComp->GiveAbility(FGameplayAbilitySpec(StartupAbility.GetDefaultObject(), 1, 0));
}

void AAOCharacter::ChangeAbilityLevelWithTags(FGameplayTagContainer TagContainer, int32 NewLevel)
{
	TArray<FGameplayAbilitySpec*> MatchingAbility;
	AbilitySystemComp->GetActivatableGameplayAbilitySpecsByAllMatchingTags(TagContainer, MatchingAbility, true);
	for (FGameplayAbilitySpec* Spec : MatchingAbility)
	{
		Spec->Level = NewLevel;
	}
}

void AAOCharacter::ApplyGEtoTarget(const FGameplayEffectSpecHandle& GESpec, const FGameplayAbilityTargetDataHandle& TargetData)
{
	for (TSharedPtr<FGameplayAbilityTargetData> Data : TargetData.Data)
		{
			Data->ApplyGameplayEffectSpec(*GESpec.Data.Get());
		}
}

void AAOCharacter::SetLootSource(class UAOInventoryComponent* NewLootSource)
{
	//if item is destroyed; stop looting
	if (NewLootSource && NewLootSource->GetOwner())
	{
		NewLootSource->GetOwner()->OnDestroyed.AddUniqueDynamic(this, &AAOCharacter::OnLootSourceDestroyed);
	}
	LootSource = NewLootSource;
	ShowHideLootMenu();
	UE_LOG(LogTemp, Warning, TEXT("set  new loot source"));

	//for AI
	  	if (NewLootSource)
	  	{
	  		if (AAOCharacter* Character = Cast<AAOCharacter>(NewLootSource->GetOwner()))
	 		{
	 			Character->SetLifeSpan(120.f);
	 		}
	  	}
	
}

bool AAOCharacter::IsLooting() const
{
	return LootSource != nullptr;
}

void AAOCharacter::LootItem(class UAOItem* ItemToGive)
{
	if (PlayerInventory && LootSource && ItemToGive && LootSource->HasItem(ItemToGive->GetClass(), ItemToGive->GetQuantity()))
	{
		const FItemAddResult AddResult = PlayerInventory->TryAddItem(ItemToGive);
		PlayerInventory->OnInventoryUpdated.Broadcast();
		if (AddResult.ActualAmountGiven > 0)
		{
			LootSource->ConsumeItem(ItemToGive, AddResult.ActualAmountGiven);
		}
		else
		{
			if (AAOPlayerController* PC = Cast<AAOPlayerController>(GetController()))
			{
				PC->ShowNotification(AddResult.ErrorText);
			}
		}
	}
}

void AAOCharacter::BeginLootingNPC(class AAOCharacter* Character)
{
	if (Character)
	{
		Character->SetLootSource(PlayerInventory);
	}
}

void AAOCharacter::OnLootSourceDestroyed(AActor* DestroyedActor)
{
	if (LootSource && DestroyedActor == LootSource->GetOwner())
	{
		SetLootSource(nullptr);
	}
}

void AAOCharacter::ShowHideLootMenu()
{
	if (AAOPlayerController* PC = Cast<AAOPlayerController>(GetController()))
	{
		if (PC)
		{
			if (LootSource)
			{
				UE_LOG(LogTemp, Warning, TEXT("On Rep!!!!!!!! new loot source"));
				PC->ShowLootMenu(LootSource);
			}
			else
			{
				PC->HideLootMenu();
			}
		}
	}
}

