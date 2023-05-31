// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AOCoreComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Attributes/AOPlayerAttributeSet.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UAOCoreComponent::UAOCoreComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UAOCoreComponent::SetupOwner()
{
	if (!GetOwner())
	{
		return;
	}

	OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	OwnerPawn = Cast<APawn>(OwnerActor);
	OwnerCharacter = Cast<ACharacter>(OwnerActor);

	AbilitySystemComp = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);
}

void UAOCoreComponent::RegisterAbilitySystemDelegates(UAbilitySystemComponent* ASC)
{
	if (!ASC)
	{
		return;
	}

	// Make sure to shutdown delegates previously registered, if RegisterAbilitySystemDelegates is called more than once (likely from AbilityActorInfo)
	ShutdownAbilitySystemDelegates(ASC);

	TArray<FGameplayAttribute> Attributes;
	ASC->GetAllAttributes(Attributes);

	for (FGameplayAttribute Attribute : Attributes)
	{
		if (Attribute == UAOPlayerAttributeSet::GetDamageAttribute() || Attribute == UAOPlayerAttributeSet::GetStaminaDamageAttribute())
		{
			ASC->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UAOCoreComponent::OnDamageAttributeChanged);
		}
		else
		{
			ASC->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UAOCoreComponent::OnAttributeChanged);
		}
	}
}

void UAOCoreComponent::ShutdownAbilitySystemDelegates(UAbilitySystemComponent* ASC)
{
	if (!ASC)
	{
		return;
	}

	TArray<FGameplayAttribute> Attributes;
	ASC->GetAllAttributes(Attributes);

	for (const FGameplayAttribute& Attribute : Attributes)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(Attribute).RemoveAll(this);
	}

	ASC->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
	ASC->OnAnyGameplayEffectRemovedDelegate().RemoveAll(this);
	ASC->RegisterGenericGameplayTagEvent().RemoveAll(this);
	ASC->AbilityCommittedCallbacks.RemoveAll(this);

}

void UAOCoreComponent::HandleDamage(float DamageAmount, const FGameplayTagContainer& DamageTags, AActor* SourceActor)
{
	OnDamage.Broadcast(DamageAmount, SourceActor, DamageTags);
}

void UAOCoreComponent::HandleHealthChange(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	OnHealthChange.Broadcast(DeltaValue, EventTags);
	if (!IsAlive())
	{
		Die();
	}
}

void UAOCoreComponent::HandleStaminaChange(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	OnStaminaChange.Broadcast(DeltaValue, EventTags);
}


void UAOCoreComponent::HandleEnergyChange(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	OnEnergyChange.Broadcast(DeltaValue, EventTags);
}

void UAOCoreComponent::HandleAttributeChange(FGameplayAttribute Attribute, float DeltaValue, const FGameplayTagContainer& EventTags)
{
	OnAttributeChange.Broadcast(Attribute, DeltaValue, EventTags);
}

void UAOCoreComponent::OnAttributeChanged(const FOnAttributeChangeData& Data)
{
	const float NewValue = Data.NewValue;
	const float OldValue = Data.OldValue;

	// Prevent broadcast Attribute changes if New and Old values are the same
	// most likely because of clamping in post gameplay effect execute
	if (OldValue == NewValue)
	{
		return;
	}

	const FGameplayEffectModCallbackData* ModData = Data.GEModData;
	FGameplayTagContainer SourceTags = FGameplayTagContainer();
	if (ModData)
	{
		SourceTags = *ModData->EffectSpec.CapturedSourceTags.GetAggregatedTags();
	}

	// Broadcast attribute change to component
	OnAttributeChange.Broadcast(Data.Attribute, NewValue - OldValue, SourceTags);
}

void UAOCoreComponent::OnDamageAttributeChanged(const FOnAttributeChangeData& Data)
{
	// if we ever need to broadcast via delegate
}

void UAOCoreComponent::Die()
{
	OnDeath.Broadcast();
}

float UAOCoreComponent::GetHealth() const
{
	if (!AbilitySystemComp)
	{
		return 0.0f;
	}

	return GetAttributeValue(UAOPlayerAttributeSet::GetHealthAttribute());
}

float UAOCoreComponent::GetMaxHealth() const
{
	if (!AbilitySystemComp)
	{
		return 0.0f;
	}
	return GetAttributeValue(UAOPlayerAttributeSet::GetMaxHealthAttribute());
}

float UAOCoreComponent::GetStamina() const
{
	if (!AbilitySystemComp)
	{
		return 0.0f;
	}
	return GetAttributeValue(UAOPlayerAttributeSet::GetStaminaAttribute());
}


float UAOCoreComponent::GetMaxStamina() const
{
	if (!AbilitySystemComp)
	{
		return 0.0f;
	}
	return GetAttributeValue(UAOPlayerAttributeSet::GetMaxStaminaAttribute());
}


float UAOCoreComponent::GetEnergy() const
{
	if (!AbilitySystemComp)
	{
		return 0.0f;
	}
	return GetAttributeValue(UAOPlayerAttributeSet::GetEnergyAttribute());
}


float UAOCoreComponent::GetMaxEnergy() const
{
	if (!AbilitySystemComp)
	{
		return 0.0f;
	}
	return GetAttributeValue(UAOPlayerAttributeSet::GetMaxEnergyAttribute());
}


float UAOCoreComponent::GetAttributeValue(FGameplayAttribute Attribute) const
{
	if (!AbilitySystemComp)
	{
		return 0.0f;
	}

	if (!AbilitySystemComp->HasAttributeSetForAttribute(Attribute))
	{
		const UObject* Owner = Cast<UObject>(this);
		const FString OwnerName = OwnerActor ? OwnerActor->GetName() : Owner->GetName();
		return 0.0f;
	}

	return AbilitySystemComp->GetNumericAttributeBase(Attribute);
}

float UAOCoreComponent::GetCurrentAttributeValue(FGameplayAttribute Attribute) const
{
	if (!AbilitySystemComp)
	{
		return 0.0f;
	}

	if (!AbilitySystemComp->HasAttributeSetForAttribute(Attribute))
	{
		const UObject* Owner = Cast<UObject>(this);
		const FString OwnerName = OwnerActor ? OwnerActor->GetName() : Owner->GetName();
		return 0.0f;
	}

	return AbilitySystemComp->GetNumericAttribute(Attribute);
}

bool UAOCoreComponent::IsAlive() const
{
	return GetHealth() > 0.0f;
}

void UAOCoreComponent::GrantAbility(TSubclassOf<UGameplayAbility> Ability, int32 Level /*= 1*/)
{
	if (!OwnerActor || !AbilitySystemComp || !Ability)
	{
		return;
	}

	if (!AbilitySystemComp->IsOwnerActorAuthoritative())
	{
		return;
	}

	FGameplayAbilitySpec Spec;
	Spec.Ability = Ability.GetDefaultObject();

	FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, Level, INDEX_NONE, OwnerActor);
	AbilitySystemComp->GiveAbility(AbilitySpec);
}

void UAOCoreComponent::ClearAbility(TSubclassOf<UGameplayAbility> Ability)
{
	TArray<TSubclassOf<UGameplayAbility>> AbilitiesToRemove;
	AbilitiesToRemove.Add(Ability);
	return ClearAbilities(AbilitiesToRemove);
}

void UAOCoreComponent::ClearAbilities(TArray<TSubclassOf<UGameplayAbility>> Abilities)
{
	if (!OwnerActor || !AbilitySystemComp || !AbilitySystemComp->IsOwnerActorAuthoritative())
	{
		return;
	}

	// Remove any abilities added from a previous call.
	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComp->GetActivatableAbilities())
	{
		if (Abilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	// Do in two passes so the removal happens after we have the full list
	for (const FGameplayAbilitySpecHandle AbilityToRemove : AbilitiesToRemove)
	{
		AbilitySystemComp->ClearAbility(AbilityToRemove);
	}
}

bool UAOCoreComponent::HasAnyMatchingGameplayTags(const FGameplayTagContainer TagContainer) const
{
	if (AbilitySystemComp)
	{
		return AbilitySystemComp->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool UAOCoreComponent::HasMatchingGameplayTag(const FGameplayTag TagToCheck) const
{
	if (AbilitySystemComp)
	{
		return AbilitySystemComp->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool UAOCoreComponent::IsUsingAbilityByClass(TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (!AbilityClass)
	{
		return false;
	}

	return GetActiveAbilitiesByClass(AbilityClass).Num() > 0;
}

bool UAOCoreComponent::IsUsingAbilityByTags(FGameplayTagContainer AbilityTags)
{
	if (!AbilitySystemComp)
	{
		return false;
	}

	return GetActiveAbilitiesByTags(AbilityTags).Num() > 0;
}

TArray<UGameplayAbility*> UAOCoreComponent::GetActiveAbilitiesByClass(TSubclassOf<UGameplayAbility> AbilityToSearch) const
{
	if (!AbilitySystemComp)
	{
		return {};
	}

	TArray<FGameplayAbilitySpec> Specs = AbilitySystemComp->GetActivatableAbilities();
	TArray<struct FGameplayAbilitySpec*> MatchingGameplayAbilities;
	TArray<UGameplayAbility*> ActiveAbilities;

	// First, search for matching Abilities for this class
	for (const FGameplayAbilitySpec& Spec : Specs)
	{
		if (Spec.Ability && Spec.Ability->GetClass()->IsChildOf(AbilityToSearch))
		{
			MatchingGameplayAbilities.Add(const_cast<FGameplayAbilitySpec*>(&Spec));
		}
	}

	// Iterate the list of all ability specs
	for (const FGameplayAbilitySpec* Spec : MatchingGameplayAbilities)
	{
		// Iterate all instances on this ability spec, which can include instance per execution abilities
		TArray<UGameplayAbility*> AbilityInstances = Spec->GetAbilityInstances();

		for (UGameplayAbility* ActiveAbility : AbilityInstances)
		{
			if (ActiveAbility->IsActive())
			{
				ActiveAbilities.Add(ActiveAbility);
			}
		}
	}

	return ActiveAbilities;
}

TArray<UGameplayAbility*> UAOCoreComponent::GetActiveAbilitiesByTags(const FGameplayTagContainer GameplayTagContainer) const
{
	if (!AbilitySystemComp)
	{
		return {};
	}

	TArray<UGameplayAbility*> ActiveAbilities;
	TArray<FGameplayAbilitySpec*> MatchingGameplayAbilities;
	AbilitySystemComp->GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer, MatchingGameplayAbilities, false);

	// Iterate the list of all ability specs
	for (const FGameplayAbilitySpec* Spec : MatchingGameplayAbilities)
	{
		// Iterate all instances on this ability spec
		TArray<UGameplayAbility*> AbilityInstances = Spec->GetAbilityInstances();

		for (UGameplayAbility* ActiveAbility : AbilityInstances)
		{
			if (ActiveAbility->IsActive())
			{
				ActiveAbilities.Add(ActiveAbility);
			}
		}
	}

	return ActiveAbilities;
}

void UAOCoreComponent::SetAttributeValue(FGameplayAttribute Attribute, float NewValue)
{
	if (!AbilitySystemComp)
	{
		return;
	}

	AbilitySystemComp->SetNumericAttributeBase(Attribute, NewValue);
}

void UAOCoreComponent::ClampAttributeValue(FGameplayAttribute Attribute, float MinValue, float MaxValue)
{
	const float CurrentValue = GetAttributeValue(Attribute);
	const float NewValue = FMath::Clamp(CurrentValue, MinValue, MaxValue);
	SetAttributeValue(Attribute, NewValue);
}

void UAOCoreComponent::AdjustAttributeForMaxChange(UAOBaseAttributeSet* AttributeSet, const FGameplayAttribute AffectedAttributeProperty, const FGameplayAttribute MaxAttribute, float NewMaxValue)
{
	if (!AbilitySystemComp)
	{
		return;
	}

	FGameplayAttributeData* AttributeData = AffectedAttributeProperty.GetGameplayAttributeData(AttributeSet);
	if (!AttributeData)
	{
		return;
	}

	const FGameplayAttributeData* MaxAttributeData = MaxAttribute.GetGameplayAttributeData(AttributeSet);
	if (!AttributeData)
	{
		return;
	}

	AttributeSet->AdjustAttributeForMaxChange(*AttributeData, *MaxAttributeData, NewMaxValue, AffectedAttributeProperty);
}

void UAOCoreComponent::PreAttributeChange(UAOBaseAttributeSet* AttributeSet, const FGameplayAttribute& Attribute, float NewValue)
{
	OnPreAttributeChange.Broadcast(AttributeSet, Attribute, NewValue);
}

void UAOCoreComponent::PostGameplayEffectExecute(UAOBaseAttributeSet* AttributeSet, const FGameplayEffectModCallbackData& Data)
{
	AActor* SourceActor = nullptr;
	AActor* TargetActor = nullptr;
	AttributeSet->GetSourceAndTargetFromContext<AActor>(Data, SourceActor, TargetActor);

	const FGameplayTagContainer SourceTags = AttributeSet->GetSourceTagsFromContext(Data);
	const FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();

	// Get Minimum Clamp value for this attribute, if it is available
	const float ClampMinimumValue = AttributeSet->GetClampMinimumValueFor(Data.EvaluatedData.Attribute);

	// Compute the delta between old and new, if it is available
	float DeltaValue = 0;
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
	{
		// If this was additive, store the raw delta value to be passed along later
		DeltaValue = Data.EvaluatedData.Magnitude;
	}

	// Delegate any attribute handling to Blueprints
	FGameplayEffectExecData Payload;
	Payload.AttributeSet = AttributeSet;
	Payload.AbilitySystemComp = AttributeSet->GetOwningAbilitySystemComponent();
	Payload.DeltaValue = DeltaValue;
	Payload.ClampMinimumValue = ClampMinimumValue;
	OnPostGameplayEffectExecute.Broadcast(Data.EvaluatedData.Attribute, SourceActor, TargetActor, SourceTags, Payload);
}

void UAOCoreComponent::SetStartupAbilitiesGranted(bool bGranted)
{
	bStartupAbilitiesGranted = bGranted;
}

void UAOCoreComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupOwner();
	
}

void UAOCoreComponent::BeginDestroy()
{
	// Clean up any bound delegates when component is destroyed
	ShutdownAbilitySystemDelegates(AbilitySystemComp);

	Super::BeginDestroy();
}


void UAOCoreComponent::OnActiveGameplayEffectAdded(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	if (!AbilitySystemComp)
	{
		return;
	}

	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);

	OnGameplayEffectAdded.Broadcast(AssetTags, GrantedTags, ActiveHandle);

	AbilitySystemComp->OnGameplayEffectStackChangeDelegate(ActiveHandle)->AddUObject(this, &UAOCoreComponent::OnActiveGameplayEffectStackChanged);
	AbilitySystemComp->OnGameplayEffectTimeChangeDelegate(ActiveHandle)->AddUObject(this, &UAOCoreComponent::OnActiveGameplayEffectTimeChanged);

	// Store active handles to clear out bound delegates when shutting down listeners
	GameplayEffectAddedHandles.AddUnique(ActiveHandle);
}

void UAOCoreComponent::OnActiveGameplayEffectStackChanged(FActiveGameplayEffectHandle ActiveHandle, int32 NewStackCount, int32 PreviousStackCount)
{
	if (!AbilitySystemComp)
	{
		return;
	}

	const FActiveGameplayEffect* GameplayEffect = AbilitySystemComp->GetActiveGameplayEffect(ActiveHandle);
	if (!GameplayEffect)
	{
		return;
	}

	FGameplayTagContainer AssetTags;
	GameplayEffect->Spec.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	GameplayEffect->Spec.GetAllGrantedTags(GrantedTags);

	OnGameplayEffectStackChange.Broadcast(AssetTags, GrantedTags, ActiveHandle, NewStackCount, PreviousStackCount);
}

void UAOCoreComponent::OnActiveGameplayEffectTimeChanged(FActiveGameplayEffectHandle ActiveHandle, float NewStartTime, float NewDuration)
{
	if (!AbilitySystemComp)
	{
		return;
	}

	const FActiveGameplayEffect* GameplayEffect = AbilitySystemComp->GetActiveGameplayEffect(ActiveHandle);
	if (!GameplayEffect)
	{
		return;
	}

	FGameplayTagContainer AssetTags;
	GameplayEffect->Spec.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	GameplayEffect->Spec.GetAllGrantedTags(GrantedTags);

	OnGameplayEffectTimeChange.Broadcast(AssetTags, GrantedTags, ActiveHandle, NewStartTime, NewDuration);
}

void UAOCoreComponent::OnAnyGameplayEffectRemoved(const FActiveGameplayEffect& EffectRemoved)
{
	if (!AbilitySystemComp)
	{
		return;
	}

	FGameplayTagContainer AssetTags;
	EffectRemoved.Spec.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	EffectRemoved.Spec.GetAllGrantedTags(GrantedTags);

	OnGameplayEffectStackChange.Broadcast(AssetTags, GrantedTags, EffectRemoved.Handle, 0, 1);
	OnGameplayEffectRemoved.Broadcast(AssetTags, GrantedTags, EffectRemoved.Handle);
}

void UAOCoreComponent::OnAnyGameplayTagChanged(FGameplayTag GameplayTag, int32 NewCount) const
{
	OnGameplayTagChange.Broadcast(GameplayTag, NewCount);
}

void UAOCoreComponent::OnAbilityCommitted(UGameplayAbility* ActivatedAbility)
{
	if (!ActivatedAbility)
	{
		return;
	}

	// Trigger AbilityCommit event
	OnAbilityCommit.Broadcast(ActivatedAbility);

	HandleCooldownOnAbilityCommit(ActivatedAbility);
}

void UAOCoreComponent::OnCooldownGameplayTagChanged(const FGameplayTag GameplayTag, int32 NewCount, FGameplayAbilitySpecHandle AbilitySpecHandle, float Duration)
{
	if (NewCount != 0)
	{
		return;
	}

	if (!AbilitySystemComp)
	{
		return;
	}

	FGameplayAbilitySpec* AbilitySpec = AbilitySystemComp->FindAbilitySpecFromHandle(AbilitySpecHandle);
	if (!AbilitySpec)
	{
		// Ability might have been cleared when cooldown expires
		return;
	}

	UGameplayAbility* Ability = AbilitySpec->Ability;

	// Broadcast cooldown expiration to BP
	if (IsValid(Ability))
	{
		OnCooldownEnd.Broadcast(Ability, GameplayTag, Duration);
	}

	AbilitySystemComp->RegisterGameplayTagEvent(GameplayTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
}

void UAOCoreComponent::HandleCooldownOnAbilityCommit(UGameplayAbility* ActivatedAbility)
{
	if (!AbilitySystemComp)
	{
		return;
	}

	if (!IsValid(ActivatedAbility))
	{
		return;
	}

	// Figure out cooldown
	UGameplayEffect* CooldownGE = ActivatedAbility->GetCooldownGameplayEffect();
	if (!CooldownGE)
	{
		return;
	}

	if (!ActivatedAbility->IsInstantiated())
	{
		return;
	}

	const FGameplayTagContainer* CooldownTags = ActivatedAbility->GetCooldownTags();
	if (!CooldownTags || CooldownTags->Num() <= 0)
	{
		return;
	}

	FGameplayAbilityActorInfo ActorInfo = ActivatedAbility->GetActorInfo();
	const FGameplayAbilitySpecHandle AbilitySpecHandle = ActivatedAbility->GetCurrentAbilitySpecHandle();

	float TimeRemaining = 0.f;
	float Duration = 0.f;
	ActivatedAbility->GetCooldownTimeRemainingAndDuration(AbilitySpecHandle, &ActorInfo, TimeRemaining, Duration);

	OnCooldownStart.Broadcast(ActivatedAbility, *CooldownTags, TimeRemaining, Duration);

	// Register delegate to monitor any change to cooldown gameplay tag to be able to figure out when a cooldown expires
	TArray<FGameplayTag> GameplayTags;
	CooldownTags->GetGameplayTagArray(GameplayTags);
	for (const FGameplayTag GameplayTag : GameplayTags)
	{
		AbilitySystemComp->RegisterGameplayTagEvent(GameplayTag)
			.AddUObject(this, &UAOCoreComponent::OnCooldownGameplayTagChanged, AbilitySpecHandle, Duration);

		GameplayTagBoundToDelegates.AddUnique(GameplayTag);
	}
}

