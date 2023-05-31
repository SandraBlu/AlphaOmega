// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AOUserWidget.h"
#include "AbilitySystemGlobals.h"
#include "Framework/AOBFL.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UAOUserWidget::SetOwnerActor(AActor* Actor)
{
	OwnerActor = Actor;
	OwnerCoreComponent = UAOBFL::GetAOCoreComponent(Actor);
	AbilitySystemComp = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
}

void UAOUserWidget::InitializeWithAbilitySystem(const UAbilitySystemComponent* AbilitySystemComponent)
{
	if (!AbilitySystemComp)
	{
		return;
	}

	if (!OwnerActor)
	{
		SetOwnerActor(AbilitySystemComp->GetOwnerActor());
	}

	if (AbilitySystemComp && AbilitySystemComp != AbilitySystemComp)
	{
		ResetAbilitySystem();
	}

	AbilitySystemComp = const_cast<UAbilitySystemComponent*>(AbilitySystemComp);
	RegisterAbilitySystemDelegates();

	// Broadcast info to Blueprints
	OnAbilitySystemInitialized();
}

void UAOUserWidget::ResetAbilitySystem()
{
	ShutdownAbilitySystemComponentListeners();
	AbilitySystemComp = nullptr;
}

void UAOUserWidget::RegisterAbilitySystemDelegates()
{
	if (!AbilitySystemComp)
	{
		// Ability System may not have been available yet for character (PlayerState setup on clients)
		return;
	}

	TArray<FGameplayAttribute> Attributes;
	AbilitySystemComp->GetAllAttributes(Attributes);

	for (FGameplayAttribute Attribute : Attributes)
	{
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UAOUserWidget::OnAttributeChanged);
	}
	// Handle GameplayEffects added / remove
	AbilitySystemComp->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UAOUserWidget::OnActiveGameplayEffectAdded);
	AbilitySystemComp->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UAOUserWidget::OnAnyGameplayEffectRemoved);

	// Handle generic GameplayTags added / removed
	AbilitySystemComp->RegisterGenericGameplayTagEvent().AddUObject(this, &UAOUserWidget::OnAnyGameplayTagChanged);
}

void UAOUserWidget::ShutdownAbilitySystemComponentListeners() const
{
	if (!AbilitySystemComp)
	{
		return;
	}

	TArray<FGameplayAttribute> Attributes;
	AbilitySystemComp->GetAllAttributes(Attributes);

	for (const FGameplayAttribute& Attribute : Attributes)
	{
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(Attribute).RemoveAll(this);
	}

	AbilitySystemComp->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
	AbilitySystemComp->OnAnyGameplayEffectRemovedDelegate().RemoveAll(this);
	AbilitySystemComp->RegisterGenericGameplayTagEvent().RemoveAll(this);
	AbilitySystemComp->AbilityCommittedCallbacks.RemoveAll(this);

}

float UAOUserWidget::GetPercentForAttributes(FGameplayAttribute Attribute, FGameplayAttribute MaxAttribute) const
{
	const float AttributeValue = GetAttributeValue(Attribute);
	const float MaxAttributeValue = GetAttributeValue(MaxAttribute);

	if (MaxAttributeValue == 0.f)
	{
		return 0.f;
	}

	return AttributeValue / MaxAttributeValue;
}

float UAOUserWidget::GetAttributeValue(FGameplayAttribute Attribute) const
{
	if (!AbilitySystemComp)
	{
		return 0.0f;
	}

	if (!AbilitySystemComp->HasAttributeSetForAttribute(Attribute))
	{
		const UClass* AttributeSet = Attribute.GetAttributeSetClass();
		return 0.0f;
	}

	return AbilitySystemComp->GetNumericAttribute(Attribute);
}

void UAOUserWidget::OnAttributeChanged(const FOnAttributeChangeData& Data)
{
	// Broadcast event to Blueprint
	OnAttributeChange(Data.Attribute, Data.NewValue, Data.OldValue);

	// Trigger post attribute change event for subclass that needs further handling
	HandleAttributeChange(Data.Attribute, Data.NewValue, Data.OldValue);
}

void UAOUserWidget::OnActiveGameplayEffectStackChanged(FActiveGameplayEffectHandle ActiveHandle, int32 NewStackCount, int32 PreviousStackCount)
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

	HandleGameplayEffectStackChange(AssetTags, GrantedTags, ActiveHandle, NewStackCount, PreviousStackCount);
}

void UAOUserWidget::OnActiveGameplayEffectTimeChanged(FActiveGameplayEffectHandle ActiveHandle, float NewStartTime, float NewDuration)
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

	HandleGameplayEffectTimeChange(AssetTags, GrantedTags, ActiveHandle, NewStartTime, NewDuration);
}

void UAOUserWidget::OnActiveGameplayEffectAdded(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);

	if (AbilitySystemComp)
	{
		AbilitySystemComp->OnGameplayEffectStackChangeDelegate(ActiveHandle)->AddUObject(this, &UAOUserWidget::OnActiveGameplayEffectStackChanged);
		AbilitySystemComp->OnGameplayEffectTimeChangeDelegate(ActiveHandle)->AddUObject(this, &UAOUserWidget::OnActiveGameplayEffectTimeChanged);

		// Store active handles to clear out bound delegates when shutting down listeners
		GameplayEffectAddedHandles.AddUnique(ActiveHandle);
	}

	HandleGameplayEffectAdded(AssetTags, GrantedTags, ActiveHandle);
}

void UAOUserWidget::OnAnyGameplayEffectRemoved(const FActiveGameplayEffect& EffectRemoved)
{
	if (!AbilitySystemComp)
	{
		return;
	}

	FGameplayTagContainer AssetTags;
	EffectRemoved.Spec.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	EffectRemoved.Spec.GetAllGrantedTags(GrantedTags);

	// Broadcast any GameplayEffect change to HUD
	HandleGameplayEffectStackChange(AssetTags, GrantedTags, EffectRemoved.Handle, 0, 1);
	HandleGameplayEffectRemoved(AssetTags, GrantedTags, EffectRemoved.Handle);
}

void UAOUserWidget::OnAnyGameplayTagChanged(FGameplayTag GameplayTag, int32 NewCount)
{
	HandleGameplayTagChange(GameplayTag, NewCount);
}

void UAOUserWidget::OnAbilityCommitted(UGameplayAbility* ActivatedAbility)
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
	const UGameplayEffect* CooldownGE = ActivatedAbility->GetCooldownGameplayEffect();
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

	const FGameplayAbilityActorInfo ActorInfo = ActivatedAbility->GetActorInfo();
	const FGameplayAbilitySpecHandle AbilitySpecHandle = ActivatedAbility->GetCurrentAbilitySpecHandle();

	float TimeRemaining = 0.f;
	float Duration = 0.f;
	ActivatedAbility->GetCooldownTimeRemainingAndDuration(AbilitySpecHandle, &ActorInfo, TimeRemaining, Duration);

	// Broadcast start of cooldown to HUD
	const FGameplayAbilitySpec* AbilitySpec = AbilitySystemComp->FindAbilitySpecFromHandle(AbilitySpecHandle);
	if (AbilitySpec)
	{
		HandleCooldownStart(AbilitySpec->Ability, *CooldownTags, TimeRemaining, Duration);
	}

	// Register delegate to monitor any change to cooldown gameplay tag to be able to figure out when a cooldown expires
	TArray<FGameplayTag> GameplayTags;
	CooldownTags->GetGameplayTagArray(GameplayTags);
	for (const FGameplayTag GameplayTag : GameplayTags)
	{
		AbilitySystemComp->RegisterGameplayTagEvent(GameplayTag).AddUObject(this, &UAOUserWidget::OnCooldownGameplayTagChanged, AbilitySpecHandle, Duration);
		GameplayTagBoundToDelegates.AddUnique(GameplayTag);
	}
}

void UAOUserWidget::OnCooldownGameplayTagChanged(const FGameplayTag GameplayTag, int32 NewCount, FGameplayAbilitySpecHandle AbilitySpecHandle, float Duration)
{
	if (NewCount != 0)
	{
		return;
	}

	if (!AbilitySystemComp)
	{
		return;
	}

	const FGameplayAbilitySpec* AbilitySpec = AbilitySystemComp->FindAbilitySpecFromHandle(AbilitySpecHandle);
	if (!AbilitySpec)
	{
		// Ability might have been cleared when cooldown expires
		return;
	}

	const UGameplayAbility* Ability = AbilitySpec->Ability;

	// Broadcast cooldown expiration to HUD
	if (IsValid(Ability))
	{
		HandleCooldownEnd(AbilitySpec->Ability, GameplayTag, Duration);
	}

	AbilitySystemComp->RegisterGameplayTagEvent(GameplayTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
}

void UAOUserWidget::HandleGameplayEffectStackChange(FGameplayTagContainer AssetTags, FGameplayTagContainer GrantedTags, FActiveGameplayEffectHandle ActiveHandle, int32 NewStackCount, int32 OldStackCount)
{
	OnGameplayEffectStackChange(AssetTags, GrantedTags, ActiveHandle, NewStackCount, OldStackCount);
}

void UAOUserWidget::HandleGameplayEffectTimeChange(FGameplayTagContainer AssetTags, FGameplayTagContainer GrantedTags, FActiveGameplayEffectHandle ActiveHandle, float NewStartTime, float NewDuration)
{
	OnGameplayEffectTimeChange(AssetTags, GrantedTags, ActiveHandle, NewStartTime, NewDuration);
}

void UAOUserWidget::HandleGameplayEffectAdded(FGameplayTagContainer AssetTags, FGameplayTagContainer GrantedTags, FActiveGameplayEffectHandle ActiveHandle)
{
	OnGameplayEffectAdded(AssetTags, GrantedTags, ActiveHandle, GetGameplayEffectUIData(ActiveHandle));
}

void UAOUserWidget::HandleGameplayEffectRemoved(FGameplayTagContainer AssetTags, FGameplayTagContainer GrantedTags, FActiveGameplayEffectHandle ActiveHandle)
{
	OnGameplayEffectRemoved(AssetTags, GrantedTags, ActiveHandle, GetGameplayEffectUIData(ActiveHandle));
}

void UAOUserWidget::HandleGameplayTagChange(FGameplayTag GameplayTag, int32 NewTagCount)
{
	OnGameplayTagChange(GameplayTag, NewTagCount);
}

void UAOUserWidget::HandleCooldownStart(UGameplayAbility* Ability, const FGameplayTagContainer CooldownTags, float TimeRemaining, float Duration)
{
	OnCooldownStart(Ability, CooldownTags, TimeRemaining, Duration);
}

void UAOUserWidget::HandleCooldownEnd(UGameplayAbility* Ability, FGameplayTag CooldownTag, float Duration)
{
	OnCooldownEnd(Ability, CooldownTag, Duration);
}

FAOGameplayEffectUIData UAOUserWidget::GetGameplayEffectUIData(FActiveGameplayEffectHandle ActiveHandle)
{
	return FAOGameplayEffectUIData(
		UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectStartTime(ActiveHandle),
		UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectTotalDuration(ActiveHandle),
		UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectExpectedEndTime(ActiveHandle),
		UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectStackCount(ActiveHandle),
		UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectStackLimitCount(ActiveHandle));
}
