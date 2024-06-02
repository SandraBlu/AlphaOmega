// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/AOBFL.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/AOPlayerState.h"
#include "UI/AOHUD.h"
#include "UI/Controllers/AOWidgetController.h"
#include "Framework/AOGameMode.h"
#include "AbilitySystemComponent.h"
#include "Interfaces/CombatInterface.h"
#include "AOGameplayEffectTypes.h"
#include "UI/AOHUD.h"
#include "AOGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"


bool UAOBFL::MakeWidgetControllerParama(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AAOHUD*& OutAOHUD)
{
if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		OutAOHUD = Cast<AAOHUD>(PC->GetHUD());
		if (OutAOHUD)
		{
			AAOPlayerState* PS = PC->GetPlayerState<AAOPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			OutWCParams.PlayerController = PC;
			OutWCParams.PlayerState = PS;
			OutWCParams.AbilitySystemComponent = ASC;
			OutWCParams.AttributeSet = AS;
			return true;
		}
	}
	return false;
}

UOverlayController* UAOBFL::GetOverlayController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAOHUD* AOHUD = nullptr;
	
	if (MakeWidgetControllerParama(WorldContextObject, WCParams, AOHUD))
	{
		return AOHUD->GetOverlayController(WCParams);
	}
	return nullptr;
}

UAOAttributeMenuController* UAOBFL::GetAttributeMenuController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAOHUD* AOHUD = nullptr;

	if (MakeWidgetControllerParama(WorldContextObject, WCParams, AOHUD))
	{
		return AOHUD->GetAttributeMenuController(WCParams);
	}
	return nullptr;
}

UAbilityMenuController* UAOBFL::GetAbilityMenuController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAOHUD* AOHUD = nullptr;

	if (MakeWidgetControllerParama(WorldContextObject, WCParams, AOHUD))
	{
		return AOHUD->GetAbilityMenuController(WCParams);
	}
	return nullptr;
}

void UAOBFL::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	AActor* AvatarActor = ASC->GetAvatarActor();
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	FCharClassInfo ClassDefaultInfo = CharacterClassInfo->GetClassInfo(CharacterClass);

	//Add Source Objects(ASC) and Effect Context and Outgoing Spec for AI Attributes Data Asset: CharacterClassInfo
	FGameplayEffectContextHandle PrimaryAttributeContextHandle = ASC->MakeEffectContext();
	PrimaryAttributeContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributeContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributeContextHandle = ASC->MakeEffectContext();
	SecondaryAttributeContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributeContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttSpecHandle.Data.Get());

	FGameplayEffectContextHandle ResistanceAttributeContextHandle = ASC->MakeEffectContext();
	ResistanceAttributeContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle ResistanceAttSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->ResistanceAttributes, Level, ResistanceAttributeContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*ResistanceAttSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributeContextHandle = ASC->MakeEffectContext();
	VitalAttributeContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalAttributeContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttSpecHandle.Data.Get());
}

UCharacterClassInfo* UAOBFL::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	AAOGameMode* AOGameMode = Cast<AAOGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AOGameMode == nullptr) return nullptr;
	return AOGameMode->CharacterClassInfo;
}

UAbilityInfo* UAOBFL::GetAbilityInfo(const UObject* WorldContextObject)
{
	AAOGameMode* AOGameMode = Cast<AAOGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AOGameMode == nullptr) return nullptr;
	return AOGameMode->AbilityInfo;
}

void UAOBFL::GiveNPCAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->SharedAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
	const FCharClassInfo& DefaultInfo = CharacterClassInfo->GetClassInfo(CharacterClass);
	for (auto AbilityClass : DefaultInfo.StartupAbilities)
	{
		if ((ASC->GetAvatarActor()->Implements<UCombatInterface>()))
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetCharacterLevel(ASC->GetAvatarActor()));
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

bool UAOBFL::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAOGameplayEffectContext* AOEffectContext = static_cast<const FAOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AOEffectContext->IsBlockedHit();
	}
	return false;
}

bool UAOBFL::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAOGameplayEffectContext* AOEffectContext = static_cast<const FAOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AOEffectContext->IsCriticalHit();
	}
	return false;
}

bool UAOBFL::IsDodgedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAOGameplayEffectContext* AOEffectContext = static_cast<const FAOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AOEffectContext->IsDodgedHit();
	}
	return false;
}

bool UAOBFL::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAOGameplayEffectContext* AOEffectContext = static_cast<const FAOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AOEffectContext->IsSuccessfulDebuff();
	}
	return false;
}

void UAOBFL::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	if (FAOGameplayEffectContext* AOEffectContext = static_cast<FAOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AOEffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UAOBFL::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
{
	if (FAOGameplayEffectContext* AOEffectContext = static_cast<FAOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AOEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UAOBFL::SetIsDodgedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsDodgedHit)
{
	if (FAOGameplayEffectContext* AOEffectContext = static_cast<FAOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AOEffectContext->SetIsDodgedHit(bInIsDodgedHit);
	}
}

void UAOBFL::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle, bool bInSuccessfulDebuff)
{
	if (FAOGameplayEffectContext* AOEffectContext = static_cast<FAOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AOEffectContext->SetIsSuccessfulDebuff(bInSuccessfulDebuff);
	}
}

void UAOBFL::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float InDamage)
{
	if (FAOGameplayEffectContext* AOEffectContext = static_cast<FAOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AOEffectContext->SetDebuffDamage(InDamage);
	}
}

void UAOBFL::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, float InDuration)
{
	if (FAOGameplayEffectContext* AOEffectContext = static_cast<FAOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AOEffectContext->SetDebuffDuration(InDuration);
	}
}

void UAOBFL::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, float InFrequency)
{
	if (FAOGameplayEffectContext* AOEffectContext = static_cast<FAOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AOEffectContext->SetDebuffFrequency(InFrequency);
	}
}

void UAOBFL::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType)
{
	if (FAOGameplayEffectContext* AOEffectContext = static_cast<FAOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		AOEffectContext->SetDamageType(DamageType);
	}
}

void UAOBFL::GetTargetsWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	TArray<FOverlapResult> Overlaps;
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}
		}
	}
}

bool UAOBFL::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bFriends = FirstActor->ActorHasTag(FName("player")) && SecondActor->ActorHasTag(FName("player")) || FirstActor->ActorHasTag(FName("enemy")) && SecondActor->ActorHasTag(FName("enemy"));
	return !bFriends;
}

int32 UAOBFL::GetXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return 0;

	const FCharClassInfo& Info = CharacterClassInfo->GetClassInfo(CharacterClass);
	const float XPReward = Info.XPReward.GetValueAtLevel(CharacterLevel);

	return static_cast<int32>(XPReward);
}

FGameplayEffectContextHandle UAOBFL::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	const FAOGameplayTags& GameplayTags = FAOGameplayTags::Get();
	const AActor* SourceAvatarActor = DamageEffectParams.SourceASC->GetAvatarActor();

	FGameplayEffectContextHandle EffectContexthandle = DamageEffectParams.SourceASC->MakeEffectContext();
	EffectContexthandle.AddSourceObject(SourceAvatarActor);
	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceASC->MakeOutgoingSpec(DamageEffectParams.DamageGameplayEffectClass, DamageEffectParams.AbilityLevel, EffectContexthandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Chance, DamageEffectParams.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Damage, DamageEffectParams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Duration, DamageEffectParams.DebuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Frequency, DamageEffectParams.DebuffFrequency);

	DamageEffectParams.TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	return EffectContexthandle;
}

float UAOBFL::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAOGameplayEffectContext* AOEffectContext = static_cast<const FAOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AOEffectContext->GetDebuffDamage();
	}
	return 0.f;
}

float UAOBFL::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAOGameplayEffectContext* AOEffectContext = static_cast<const FAOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AOEffectContext->GetDebuffDuration();
	}
	return 0.f;
}

float UAOBFL::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAOGameplayEffectContext* AOEffectContext = static_cast<const FAOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AOEffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

FGameplayTag UAOBFL::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAOGameplayEffectContext* AOEffectContext = static_cast<const FAOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (AOEffectContext->GetDamageType().IsValid())
		{
			return *AOEffectContext->GetDamageType();
		}
	}
	return FGameplayTag();
}
