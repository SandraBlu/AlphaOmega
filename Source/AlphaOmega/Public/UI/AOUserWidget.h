// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttributeSet.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffectTypes.h"
#include "AOUserWidget.generated.h"

class UAOCoreComponent;


USTRUCT(BlueprintType)
struct FAOGameplayEffectUIData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AOGameplayEffect")
	float StartTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AOGameplayEffect")
	float TotalDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AOGameplayEffect")
	float ExpectedEndTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AOGameplayEffect")
	int32 StackCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AOGameplayEffect")
	int32 StackLimitCount;

	FAOGameplayEffectUIData(const float StartTime, const float TotalDuration, const float ExpectedEndTime, const int32 StackCount, const int32 StackLimitCount)
		: StartTime(StartTime),
		  TotalDuration(TotalDuration),
		  ExpectedEndTime(ExpectedEndTime),
		  StackCount(StackCount),
		  StackLimitCount(StackLimitCount)
	{
	}

	FAOGameplayEffectUIData(): StartTime(0), TotalDuration(0), ExpectedEndTime(0), StackCount(0), StackLimitCount(0)
	{
	}
};

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API UAOUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly, Category = "UI")
		AActor* OwnerActor;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
		UAOCoreComponent* OwnerCoreComponent;

	/** Initialize or update references to owner actor and additional actor components (such as AbilitySystemComponent) and cache them for this instance of user widget. */
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void SetOwnerActor(AActor* Actor);

	/** Returns reference to OwnerActor for this user widget, if it has been initialized. */
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual AActor* GetOwningActor() const { return OwnerActor; }

	/** Returns reference to OwnerCoreComponent for this user widget, if it has been initialized. */
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual UAOCoreComponent* GetOwningCoreComponent() const { return OwnerCoreComponent; }

	/** Returns reference to AbilitySystemComponent for this user widget, if it has been initialized. */
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual UAbilitySystemComponent* GetOwningAbilitySystemComponent() const { return AbilitySystemComp; }

	/**
	 * Runs initialization logic for this UserWidget related to interactions with Ability System Component.
	 *
	 * Setup AbilitySystemComponent delegates to react to various events.
	 *
	 * Usually called from NativeConstruct, but exposed there if other classes needs to run initialization logic again to update references.
	 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void InitializeWithAbilitySystem(UPARAM(ref) const UAbilitySystemComponent* AbilitySystemComponent);

	/** Clears off any ASC delegates and dispose AbilitySystemComponent pointer */
	void ResetAbilitySystem();

	/** Register listeners for AbilitySystemComponent (Attributes, GameplayEffects / Tags, Cooldowns, ...) */
	virtual void RegisterAbilitySystemDelegates();

	/** Clear all delegates for AbilitySystemComponent bound to this UserWidget */
	virtual void ShutdownAbilitySystemComponentListeners() const;

	/**
	 * Event triggered when this widget has been initialized with a valid ASC.
	 *
	 * Run initialization logic that depends on Ability System here. (like setting initial state for user widgets based on attributes value)
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnAbilitySystemInitialized();

	/** Event triggered whenever an attribute value is changed on Owner Actor's ASC */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnAttributeChange(FGameplayAttribute Attribute, float NewValue, float OldValue);

	/** Event triggered by Companion Core Component whenever a gameplay effect time is changed (for instance when duration is refreshed) */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnGameplayEffectTimeChange(FGameplayTagContainer AssetTags, FGameplayTagContainer GrantedTags, FActiveGameplayEffectHandle ActiveHandle, float NewStartTime, float NewDuration);

	/** Event triggered by Companion Core Component whenever a gameplay effect is added */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnGameplayEffectAdded(FGameplayTagContainer AssetTags, FGameplayTagContainer GrantedTags, FActiveGameplayEffectHandle ActiveHandle, FAOGameplayEffectUIData EffectData);

	/** Event triggered by Companion Core Component whenever a gameplay effect is added / removed */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnGameplayEffectStackChange(FGameplayTagContainer AssetTags, FGameplayTagContainer GrantedTags, FActiveGameplayEffectHandle ActiveHandle, int32 NewStackCount, int32 OldStackCount);

	/** Event triggered by Companion Core Component whenever a gameplay effect is removed */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnGameplayEffectRemoved(FGameplayTagContainer AssetTags, FGameplayTagContainer GrantedTags, FActiveGameplayEffectHandle ActiveHandle, FAOGameplayEffectUIData EffectData);

	/** Event triggered by Companion Core Component whenever a tag is added or removed (but not if just count is increased. Only for 'new' and 'removed' events) */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnGameplayTagChange(FGameplayTag GameplayTag, int32 NewTagCount);

	/** Event triggered by Companion Core component when an ability with a valid cooldown is committed and cooldown is applied */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
		void OnCooldownStart(UGameplayAbility* Ability, const FGameplayTagContainer CooldownTags, float TimeRemaining, float Duration);

	/** Event triggered by Companion Core Component when a cooldown gameplay tag is removed, meaning cooldown expired */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
		void OnCooldownEnd(UGameplayAbility* Ability, FGameplayTag CooldownTag, float Duration);

	/** Helper function to return percentage from Attribute / MaxAttribute */
	UFUNCTION(BlueprintPure, Category = "UI")
	float GetPercentForAttributes(FGameplayAttribute Attribute, FGameplayAttribute MaxAttribute) const;

	/** Returns the current value of an attribute (base value) from owning actor Ability System (if it has any). That is, the value of the attribute with no stateful modifiers */
	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual float GetAttributeValue(FGameplayAttribute Attribute) const;

	/** Triggered by ASC and handle / broadcast Attributes change */
	virtual void OnAttributeChanged(const FOnAttributeChangeData& Data);

	/** Triggered by ASC when GEs stack count changes */
	virtual void OnActiveGameplayEffectStackChanged(FActiveGameplayEffectHandle ActiveHandle, int32 NewStackCount, int32 PreviousStackCount);

	/** Triggered by ASC when GEs stack count changes */
	virtual void OnActiveGameplayEffectTimeChanged(FActiveGameplayEffectHandle ActiveHandle, float NewStartTime, float NewDuration);

	/** Triggered by ASC when GEs are added */
	virtual void OnActiveGameplayEffectAdded(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);

	/** Triggered by ASC when any GEs are removed */
	virtual void OnAnyGameplayEffectRemoved(const FActiveGameplayEffect& EffectRemoved);

	/** Trigger by ASC when any gameplay tag is added or removed (but not if just count is increased. Only for 'new' and 'removed' events) */
	virtual void OnAnyGameplayTagChanged(FGameplayTag GameplayTag, int32 NewCount);

	/** Trigger by ASC when an ability is committed (cost / cooldown are applied) */
	virtual void OnAbilityCommitted(UGameplayAbility* ActivatedAbility);

	/** Trigger by ASC when a cooldown tag is changed (new or removed)  */
	virtual void OnCooldownGameplayTagChanged(const FGameplayTag GameplayTag, int32 NewCount, FGameplayAbilitySpecHandle AbilitySpecHandle, float Duration);

	/** Post attribute change hook for subclass that needs further handling */
	virtual void HandleAttributeChange(FGameplayAttribute Attribute, float NewValue, float OldValue) {}

	/** Trigger from ASC whenever a gameplay effect is added or removed */
	virtual void HandleGameplayEffectStackChange(FGameplayTagContainer AssetTags, FGameplayTagContainer GrantedTags, FActiveGameplayEffectHandle ActiveHandle, int32 NewStackCount, int32 OldStackCount);

	/** Trigger from ASC whenever a gameplay effect time is changed */
	virtual void HandleGameplayEffectTimeChange(FGameplayTagContainer AssetTags, FGameplayTagContainer GrantedTags, FActiveGameplayEffectHandle ActiveHandle, float NewStartTime, float NewDuration);

	/** Trigger from ASC whenever a gameplay effect is added */
	virtual void HandleGameplayEffectAdded(FGameplayTagContainer AssetTags, FGameplayTagContainer GrantedTags, FActiveGameplayEffectHandle ActiveHandle);

	/** Trigger from ASC whenever a gameplay effect is removed */
	virtual void HandleGameplayEffectRemoved(FGameplayTagContainer AssetTags, FGameplayTagContainer GrantedTags, FActiveGameplayEffectHandle ActiveHandle);

	/** Trigger from ASC whenever a gameplay tag is added or removed */
	virtual void HandleGameplayTagChange(FGameplayTag GameplayTag, int32 NewTagCount);

	/** Trigger from ASC whenever an ability with valid cooldown tags is committed (cooldown start) */
	virtual void HandleCooldownStart(UGameplayAbility* Ability, const FGameplayTagContainer CooldownTags, float TimeRemaining, float Duration);

	/** Trigger from ASC whenever an cooldown tag stack changes, and stack count is 0 (cooldown end) */
	virtual void HandleCooldownEnd(UGameplayAbility* Ability, FGameplayTag CooldownTag, float Duration);


	FAOGameplayEffectUIData GetGameplayEffectUIData(FActiveGameplayEffectHandle ActiveHandle);

protected:

	UPROPERTY()
		UAbilitySystemComponent* AbilitySystemComp;

private:

	/** Array of active GE handle bound to delegates that will be fired when the count for the key tag changes to or away from zero */
	TArray<FActiveGameplayEffectHandle> GameplayEffectAddedHandles;

	/** Array of tags bound to delegates that will be fired when the count for the key tag changes to or away from zero */
	TArray<FGameplayTag> GameplayTagBoundToDelegates;

};
