// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "UI/AOUserHUD.h"
#include "AOCoreComponent.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UAbilitySystemComponent;
class UBaseAttributeSet;
struct FGameplayAbilitySpecHandle;

/** Structure passed down to Actors Blueprint with PostGameplayEffectExecute Event */
USTRUCT(BlueprintType)
struct FGameplayEffectExecData
{
	GENERATED_BODY()

	/** The owner AttributeSet from which the event was invoked */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttributeSetPayload)
	UAttributeSet* AttributeSet = nullptr;

	/** The owner AbilitySystemComponent for this AttributeSet */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttributeSetPayload)
	UAbilitySystemComponent* AbilitySystemComp = nullptr;

	/** Calculated DeltaValue from OldValue to NewValue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttributeSetPayload)
	float DeltaValue = 0.f;

	/** The configured Clamp MinimumValue for this Attribute, if defined */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttributeSetPayload)
	float ClampMinimumValue = 0.f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInitAbilityActorInfoCore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDefaultAttributeChange, float, DeltaValue, const struct FGameplayTagContainer, EventTags);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttributeChange, FGameplayAttribute, Attribute, float, DeltaValue, const struct FGameplayTagContainer, EventTags);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPreAttributeChange, UAOBaseAttributeSet*, AttributeSet, FGameplayAttribute, Attribute, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnPostGameplayEffectExecute, FGameplayAttribute, Attribute, AActor*, Instigator, AActor*, TargetActor, const FGameplayTagContainer&, SourceTags, const FGameplayEffectExecData, Payload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDamage, float, DamageAmount, AActor*, Instigator, const struct FGameplayTagContainer&, DamageTags);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityActivated, const UGameplayAbility*, Ability);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityEnded, const UGameplayAbility*, Ability);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityFailed, const UGameplayAbility*, Ability, const FGameplayTagContainer&, ReasonTags);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnGameplayEffectStackChange, FGameplayTagContainer, AssetTags, FGameplayTagContainer, GrantedTags, FActiveGameplayEffectHandle, ActiveHandle, int32, NewStackCount, int32, OldStackCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGameplayEffectAdded, FGameplayTagContainer, AssetTags, FGameplayTagContainer, GrantedTags, FActiveGameplayEffectHandle, ActiveHandle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGameplayEffectRemoved, FGameplayTagContainer, AssetTags, FGameplayTagContainer, GrantedTags, FActiveGameplayEffectHandle, ActiveHandle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameplayTagStackChange, FGameplayTag, GameplayTag, int32, NewTagCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityCommit, UGameplayAbility*, Ability);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnCooldownChanged, UGameplayAbility*, Ability, const FGameplayTagContainer, CooldownTags, float, TimeRemaining, float, Duration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCooldownEnd, UGameplayAbility*, Ability, FGameplayTag, CooldownTag, float, Duration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnGameplayEffectTimeChange, FGameplayTagContainer, AssetTags, FGameplayTagContainer, GrantedTags, FActiveGameplayEffectHandle, ActiveHandle, float, NewStartTime, float, NewDuration);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALPHAOMEGA_API UAOCoreComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UAOCoreComponent();

	UPROPERTY()
		AActor* OwnerActor;

	UPROPERTY()
		APawn* OwnerPawn;

	UPROPERTY()
		ACharacter* OwnerCharacter;

	UPROPERTY()
		UAbilitySystemComponent* AbilitySystemComp;

	/** Setup GetOwner to character and sets references for ability system component and the owner itself. */
	void SetupOwner();

	/** Register Ability System delegates to mainly broadcast blueprint assignable event to BPs */
	void RegisterAbilitySystemDelegates(UAbilitySystemComponent* ASC);

	/** Clean up any bound delegates to Ability System delegates */
	void ShutdownAbilitySystemDelegates(UAbilitySystemComponent* ASC);

	// Called from AttributeSet, and trigger BP events
	virtual void HandleDamage(float DamageAmount, const FGameplayTagContainer& DamageTags, AActor* SourceActor);
	virtual void HandleHealthChange(float DeltaValue, const FGameplayTagContainer& EventTags);
	virtual void HandleStaminaChange(float DeltaValue, const FGameplayTagContainer& EventTags);
	virtual void HandleEnergyChange(float DeltaValue, const FGameplayTagContainer& EventTags);
	virtual void HandleAttributeChange(FGameplayAttribute Attribute, float DeltaValue, const FGameplayTagContainer& EventTags);

	//Event called just after InitAbilityActorInfo on ASC, once abilities and attributes have been granted.
	UPROPERTY(BlueprintAssignable, Category = "Abilities")
	FOnInitAbilityActorInfoCore OnInitAbilityActorInfo;

	// Called when character takes damage, which may have killed them
	UPROPERTY(BlueprintAssignable, Category = "Abilities")
	FOnDamage OnDamage;

	UPROPERTY(BlueprintAssignable, Category = "Abilities")
	FOnDefaultAttributeChange OnHealthChange;

	UPROPERTY(BlueprintAssignable, Category = "Abilities")
	FOnDefaultAttributeChange OnStaminaChange;

	UPROPERTY(BlueprintAssignable, Category = "Abilities")
	FOnDefaultAttributeChange OnEnergyChange;

	//* Called when any of the attributes owned by this character are changed
	UPROPERTY(BlueprintAssignable, Category = "Abilities")
	FOnAttributeChange OnAttributeChange;

	//Called on Death
	UPROPERTY(BlueprintAssignable, Category = "Character")
	FOnDeath OnDeath;

	// Generic Attribute change callback for attributes
	virtual void OnAttributeChanged(const FOnAttributeChangeData& Data);

	// Generic "meta" Attribute change callback for damage attributes
	virtual void OnDamageAttributeChanged(const FOnAttributeChangeData& Data);
	/**
	* Triggers death events for the owner actor
	*/
	UFUNCTION(BlueprintCallable, Category = "Character")
		virtual void Die();

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual float GetEnergy() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual float GetMaxEnergy() const;

	/** Returns the current value of an attribute (base value). That is, the value of the attribute with no stateful modifiers */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual float GetAttributeValue(FGameplayAttribute Attribute) const;

	/** Returns current (final) value of an attribute */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual float GetCurrentAttributeValue(FGameplayAttribute Attribute) const;

	/** Returns whether or not the Actor is considered alive (Health > 0) */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual bool IsAlive() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Abilities")
		virtual void GrantAbility(TSubclassOf<UGameplayAbility> Ability, int32 Level = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Abilities")
		virtual void ClearAbility(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Abilities")
		virtual void ClearAbilities(TArray<TSubclassOf<UGameplayAbility>> Abilities);


	//Returns true if Actor's AbilitySystemComponent has any of the matching tags (expands to include parents of asset tags)
	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer TagContainer) const;

	//Returns true if Actor's AbilitySystemComponent has a gameplay tag that matches against the specified tag (expands to include parents of asset tags)
	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	virtual bool HasMatchingGameplayTag(const FGameplayTag TagToCheck) const;

 	// Returns whether one of the actors's active abilities are matching the provided Ability Class * /
 	UFUNCTION(BlueprintPure, Category = "Abilities")
 	bool IsUsingAbilityByClass(TSubclassOf<UGameplayAbility> AbilityClass);
 
 	//Returns whether one of the character's active abilities are matching the provided tags * /
 	UFUNCTION(BlueprintPure, Category = "Abilities")
 	bool IsUsingAbilityByTags(FGameplayTagContainer AbilityTags);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	TArray<UGameplayAbility*> GetActiveAbilitiesByClass(TSubclassOf<UGameplayAbility> AbilityToSearch) const;
	
	//Returns a list of currently active ability instances that match the given tags if active
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	virtual TArray<UGameplayAbility*> GetActiveAbilitiesByTags(const FGameplayTagContainer GameplayTagContainer) const;

	/** Sets the base value of an attribute. Existing active modifiers are NOT cleared and will act upon the new base value. */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual void SetAttributeValue(FGameplayAttribute Attribute, float NewValue);

	/** Clamps the Attribute from MinValue to MaxValue */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual void ClampAttributeValue(FGameplayAttribute Attribute, float MinValue, float MaxValue);


	//Helper function to proportionally adjust the value of an attribute when it's associated max attribute changes.
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual void AdjustAttributeForMaxChange(UPARAM(ref) UAOBaseAttributeSet* AttributeSet, const FGameplayAttribute AffectedAttributeProperty, const FGameplayAttribute MaxAttribute, float NewMaxValue);

	/* AttributeSet Events */
	virtual void PreAttributeChange(UAOBaseAttributeSet* AttributeSet, const FGameplayAttribute& Attribute, float NewValue);
	virtual void PostGameplayEffectExecute(UAOBaseAttributeSet* AttributeSet, const FGameplayEffectModCallbackData& Data);


	//PostGameplayEffectExecute event fired off from native AttributeSets, define here
	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnPostGameplayEffectExecute OnPostGameplayEffectExecute;

	// PreAttributeChange event fired off from native AttributeSets, react here to
	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnPreAttributeChange OnPreAttributeChange;


	//Notify component that ASC abilities have been granted or not.
	//bStartupAbilitiesGranted is used to gate the triggering of some events like HealthChange or OnDeath.
	void SetStartupAbilitiesGranted(bool bGranted);

	UPROPERTY(BlueprintAssignable, Category = "GASAbility")
		FOnAbilityActivated OnAbilityActivated;

	/**
	* Called when an ability is ended for the owner actor.
	*/
	UPROPERTY(BlueprintAssignable, Category = "GASAbility")
		FOnAbilityEnded OnAbilityEnded;

	/**
	* Called when an ability failed to activated for the owner actor, passes along the failed ability
	* and a tag explaining why.
	*/
	UPROPERTY(BlueprintAssignable, Category = "GASAbility")
		FOnAbilityFailed OnAbilityFailed;

	/**
	* Called when a GameplayEffect is added or removed.
	*/
	UPROPERTY(BlueprintAssignable, Category = "GASAbility")
		FOnGameplayEffectStackChange OnGameplayEffectStackChange;

	/**
	* Called when a GameplayEffect duration is changed (for instance when duration is refreshed)
	*/
	UPROPERTY(BlueprintAssignable, Category = "GASAbility")
		FOnGameplayEffectTimeChange OnGameplayEffectTimeChange;

	/**
	* Called when a GameplayEffect is added.
	*/
	UPROPERTY(BlueprintAssignable, Category = "GASAbility")
		FOnGameplayEffectAdded OnGameplayEffectAdded;

	/**
	* Called when a GameplayEffect is removed.
	*/
	UPROPERTY(BlueprintAssignable, Category = "GASAbility")
		FOnGameplayEffectRemoved OnGameplayEffectRemoved;

	/** Called whenever a tag is added or removed (but not if just count is increased. Only for 'new' and 'removed' events) */
	UPROPERTY(BlueprintAssignable, Category = "GASAbility")
		FOnGameplayTagStackChange OnGameplayTagChange;

	/** Called whenever an ability is committed (cost / cooldown are applied) */
	UPROPERTY(BlueprintAssignable, Category = "GASAbility")
		FOnAbilityCommit OnAbilityCommit;

	/** Called when an ability with a valid cooldown is committed and cooldown is applied */
	UPROPERTY(BlueprintAssignable, Category = "GASAbility")
		FOnCooldownChanged OnCooldownStart;

	/** Called when a cooldown gameplay tag is removed, meaning cooldown expired */
	UPROPERTY(BlueprintAssignable, Category = "GASAbility")
		FOnCooldownEnd OnCooldownEnd;

protected:
	
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

	bool bStartupAbilitiesGranted = false;

	/** Triggered by ASC when GEs are added */
	virtual void OnActiveGameplayEffectAdded(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);

	/** Triggered by ASC when GEs stack count changes */
	virtual void OnActiveGameplayEffectStackChanged(FActiveGameplayEffectHandle ActiveHandle, int32 NewStackCount, int32 PreviousStackCount);

	/** Triggered by ASC when GEs stack count changes */
	virtual void OnActiveGameplayEffectTimeChanged(FActiveGameplayEffectHandle ActiveHandle, float NewStartTime, float NewDuration);

	/** Triggered by ASC when any GEs are removed */
	virtual void OnAnyGameplayEffectRemoved(const FActiveGameplayEffect& EffectRemoved);

	/** Trigger by ASC when any gameplay tag is added or removed (but not if just count is increased. Only for 'new' and 'removed' events) */
	void OnAnyGameplayTagChanged(FGameplayTag GameplayTag, int32 NewCount) const;

	/** Trigger by ASC when an ability is committed (cost / cooldown are applied)  */
	void OnAbilityCommitted(UGameplayAbility* ActivatedAbility);

	/** Trigger by ASC when a cooldown tag is changed (new or removed)  */
	virtual void OnCooldownGameplayTagChanged(const FGameplayTag GameplayTag, int32 NewCount, FGameplayAbilitySpecHandle AbilitySpecHandle, float Duration);

	/** Manage cooldown events trigger when an ability is committed */
	void HandleCooldownOnAbilityCommit(UGameplayAbility* ActivatedAbility);

private:
	/** Array of active GE handle bound to delegates that will be fired when the count for the key tag changes to or away from zero */
	TArray<FActiveGameplayEffectHandle> GameplayEffectAddedHandles;

	/** Array of tags bound to delegates that will be fired when the count for the key tag changes to or away from zero */
	TArray<FGameplayTag> GameplayTagBoundToDelegates;
};
