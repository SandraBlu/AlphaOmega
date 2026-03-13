// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilityTypes.h"
#include "GameplayEffectTypes.h"
#include "OGameplayTags.h"
#include "Engine/OverlapResult.h"
#include "Framework/CountdownAction.h"
#include "Framework/OGameModeBase.h"
#include "GAS/OAbilitySystemComponent.h"
#include "GAS/Data/CharacterClassInfo.h"
#include "Interface/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
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

void UAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
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
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

UCharacterClassInfo* UAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AOGameModeBase* GameMode = Cast<AOGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (GameMode == nullptr) return nullptr;
	return GameMode->CharacterClassInfo;
}

UAbilityInfo* UAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const AOGameModeBase* GameMode = Cast<AOGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (GameMode == nullptr) return nullptr;
	return GameMode->AbilityInfo;
}

bool UAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOGameplayEffectContext* EffectContext = static_cast<const FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->IsBlockedHit();
	}
	return false;
}

bool UAbilitySystemLibrary::IsDodgedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOGameplayEffectContext* EffectContext = static_cast<const FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->IsDodgedHit();
	}
	return false;
}

bool UAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOGameplayEffectContext* EffectContext = static_cast<const FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->IsCriticalHit();
	}
	return false;
}

bool UAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOGameplayEffectContext* EffectContext = static_cast<const FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->IsSuccessfulDebuff();
	}
	return false;
}

float UAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOGameplayEffectContext* EffectContext = static_cast<const FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->GetDebuffDamage();
	}
	return 0.f;
}

float UAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOGameplayEffectContext* EffectContext = static_cast<const FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->GetDebuffDuration();
	}
	return 0.f;
}

float UAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOGameplayEffectContext* EffectContext = static_cast<const FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

FGameplayTag UAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOGameplayEffectContext* EffectContext = static_cast<const FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (EffectContext->GetDamageType().IsValid())
		{
			return *EffectContext->GetDamageType();
		}
	}
	return FGameplayTag();
}

FVector UAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOGameplayEffectContext* EffectContext = static_cast<const FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

FVector UAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOGameplayEffectContext* EffectContext = static_cast<const FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->GetKnockbackForce();
	}
	return FVector::ZeroVector;
}

bool UAbilitySystemLibrary::IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOGameplayEffectContext* EffectContext = static_cast<const FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->IsRadialDamage();
	}
	return false;
}

float UAbilitySystemLibrary::GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOGameplayEffectContext* EffectContext = static_cast<const FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->GetRadialDamageInnerRadius();
	}
	return false;
}

float UAbilitySystemLibrary::GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOGameplayEffectContext* REffectContext = static_cast<const FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return REffectContext->GetRadialDamageOuterRadius();
	}
	return 0.f;
}

FVector UAbilitySystemLibrary::GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOGameplayEffectContext* REffectContext = static_cast<const FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return REffectContext->GetRadialDamageOrigin();
	}
	return FVector::ZeroVector;
}

void UAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	if (FOGameplayEffectContext* EffectContext = static_cast<FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UAbilitySystemLibrary::SetIsDodgedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsDodgedHit)
{
	if (FOGameplayEffectContext* EffectContext = static_cast<FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetIsDodgedHit(bInIsDodgedHit);
	}
}

void UAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
{
	if (FOGameplayEffectContext* EffectContext = static_cast<FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UAbilitySystemLibrary::SetIsSucessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle, bool bInSuccessfulDebuff)
{
	if (FOGameplayEffectContext* EffectContext = static_cast<FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetIsSuccessfulDebuff(bInSuccessfulDebuff);
	}
}

void UAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float InDebuffDamage)
{
	if (FOGameplayEffectContext* EffectContext = static_cast<FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetDebuffDamage(InDebuffDamage);
	}
}

void UAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, float bInDebuffDuration)
{
	if (FOGameplayEffectContext* EffectContext = static_cast<FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetDebuffDuration(bInDebuffDuration);
	}
}

void UAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, float bInDebuffFrequency)
{
	if (FOGameplayEffectContext* EffectContext = static_cast<FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetDebuffFrequency(bInDebuffFrequency);
	}
}

void UAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType)
{
	if (FOGameplayEffectContext* EffectContext = static_cast<FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		EffectContext->SetDamageType(DamageType);
	}
}

void UAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InImpulse)
{
	if (FOGameplayEffectContext* EffectContext = static_cast<FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetDeathImpulse(InImpulse);
	}
}

void UAbilitySystemLibrary::SetKnockbackForce(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InForce)
{
	if (FOGameplayEffectContext* EffectContext = static_cast<FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetKnockbackForce(InForce);
	}
}

void UAbilitySystemLibrary::SetIsRadialDamage(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsRadialDamage)
{
	if (FOGameplayEffectContext* EffectContext = static_cast<FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetIsRadialDamage(bInIsRadialDamage);
	}
}

void UAbilitySystemLibrary::SetRadialDamageInnerRadius(FGameplayEffectContextHandle& EffectContextHandle, float InInnerRadius)
{
	if (FOGameplayEffectContext* EffectContext = static_cast<FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetRadialDamageInnerRadius(InInnerRadius);
	}
}

void UAbilitySystemLibrary::SetRadialDamageOuterRadius(FGameplayEffectContextHandle& EffectContextHandle, float InOuterRadius)
{
	if (FOGameplayEffectContext* EffectContext = static_cast<FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetRadialDamageOuterRadius(InOuterRadius);
	}
}

void UAbilitySystemLibrary::SetRadialDamageOrigin(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InOrigin)
{
	if (FOGameplayEffectContext* EffectContext = static_cast<FOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetRadialDamageOrigin(InOrigin);
	}
}

void UAbilitySystemLibrary::GetTargetsWithinRadius(const UObject* WorldContextObject,TArray<AActor*>& OutOverlappingActors,
	const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin)
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

bool UAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bPlayerTeam = FirstActor->ActorHasTag(FName("player")) && SecondActor->ActorHasTag(FName("player"));
	const bool bEnemyTeam = FirstActor->ActorHasTag(FName("enemy")) && SecondActor->ActorHasTag(FName("enemy"));
	const bool bFriend = bPlayerTeam || bEnemyTeam;
	return !bFriend;
}

int32 UAbilitySystemLibrary::GetXPRewardForEnemySlay(const UObject* WorldContextObject, ECharacterClass CharacterClass,int32 CharacterLevel)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return 0;
	const FCharClassInfo& Info = CharacterClassInfo->GetClassInfo(CharacterClass);
	const float XPReward = Info.XPReward.GetValueAtLevel(CharacterLevel);

	return static_cast<int32>(XPReward);
}

FGameplayEffectContextHandle UAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	const FOGameplayTags& GameplayTags = FOGameplayTags::Get();
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();

	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);
	SetDeathImpulse(EffectContextHandle, DamageEffectParams.DeathImpulse);
	SetKnockbackForce(EffectContextHandle, DamageEffectParams.KnockbackForce);
	SetIsRadialDamage(EffectContextHandle, DamageEffectParams.bIsRadialDamage);
	SetRadialDamageInnerRadius(EffectContextHandle, DamageEffectParams.RadialDamageInnerRadius);
	SetRadialDamageOuterRadius(EffectContextHandle, DamageEffectParams.RadialDamageOuterRadius);
	SetRadialDamageOrigin(EffectContextHandle, DamageEffectParams.RadialDamageOrigin);
	
	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(DamageEffectParams.DamageGameplayEffectClass, DamageEffectParams.AbilityLevel, EffectContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Chance, DamageEffectParams.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Damage, DamageEffectParams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Duration, DamageEffectParams.DebuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Frequency, DamageEffectParams.DebuffFrequency);

	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	return EffectContextHandle;
}

TArray<FRotator> UAbilitySystemLibrary::EvenlySpacedRotators(const FVector& Forward, const FVector& Axis, float Spread,int32 NumRotators)
{
	TArray<FRotator> Rotators;
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	if (NumRotators > 1)
	{
		const float DeltaSpread = Spread / (NumRotators - 1);
		for (int32 i = 0; i < NumRotators; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			Rotators.Add(Direction.Rotation());
		}	
	}
	else
	{
		Rotators.Add(Forward.Rotation());
	}
	return Rotators;
}

TArray<FVector> UAbilitySystemLibrary::EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 NumVectors)
{
	TArray<FVector> Vectors;
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	if (NumVectors > 1)
	{
		const float DeltaSpread = Spread / (NumVectors - 1);
		for (int32 i = 0; i < NumVectors; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			Vectors.Add(Direction);
		}	
	}
	else
	{
		Vectors.Add(Forward);
	}
	return Vectors;
}

void UAbilitySystemLibrary::SetIsRadialDamageEffectParam(FDamageEffectParams& DamageEffectParams, bool bIsRadial,
	float InnerRadius, float OuterRadius, FVector Origin)
{
	DamageEffectParams.bIsRadialDamage = bIsRadial;
	DamageEffectParams.RadialDamageInnerRadius = InnerRadius;
	DamageEffectParams.RadialDamageOuterRadius = OuterRadius;
	DamageEffectParams.RadialDamageOrigin = Origin;
}

void UAbilitySystemLibrary::SetKnockbackDirection(FDamageEffectParams& DamageEffectParams, FVector KnockbackDirection,
	float Magnitude)
{
	KnockbackDirection.Normalize();
	if (Magnitude == 0.f)
	{
		DamageEffectParams.KnockbackForce = KnockbackDirection * DamageEffectParams.KnockbackMagnitude;
	}
	else
	{
		DamageEffectParams.KnockbackForce = KnockbackDirection * Magnitude;
	}
}

void UAbilitySystemLibrary::SetDeathImpulseDirection(FDamageEffectParams& DamageEffectParams, FVector ImpulseDirection,float Magnitude)
{
	ImpulseDirection.Normalize();
	if (Magnitude == 0.f)
	{
		DamageEffectParams.DeathImpulse = ImpulseDirection * DamageEffectParams.DeathImpulseMagnitude;
	}
	else
	{
		DamageEffectParams.DeathImpulse = ImpulseDirection * Magnitude;
	}
}

void UAbilitySystemLibrary::SetTargetEffectParamsASC(FDamageEffectParams& DamageEffectParams,UAbilitySystemComponent* InASC)
{
	DamageEffectParams.TargetAbilitySystemComponent = InASC;
}

UOAbilitySystemComponent* UAbilitySystemLibrary::NativeGetRASCFromActor(AActor* InActor)
{
	check(InActor);
	return CastChecked<UOAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

void UAbilitySystemLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	UOAbilitySystemComponent* ASC = NativeGetRASCFromActor(InActor);
	if (!ASC->HasMatchingGameplayTag(TagToAdd))
	{
		ASC->AddLooseGameplayTag(TagToAdd);
	}
}

void UAbilitySystemLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
	UOAbilitySystemComponent* ASC = NativeGetRASCFromActor(InActor);
	if (ASC->HasMatchingGameplayTag(TagToRemove))
	{
		ASC->RemoveLooseGameplayTag(TagToRemove);
	}
}

bool UAbilitySystemLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
{
	UOAbilitySystemComponent* ASC = NativeGetRASCFromActor(InActor);
	return ASC->HasMatchingGameplayTag(TagToCheck);
}

void UAbilitySystemLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EConfirmType& OutConfirmType)
{
	OutConfirmType = NativeDoesActorHaveTag(InActor,TagToCheck)? EConfirmType::Yes : EConfirmType::No;
}

UCombatComponent* UAbilitySystemLibrary::NativeGetCombatComponentFromActor(AActor* InActor)
{
	check(InActor);
	if (ICombatInterface* PawnCombatInterface = Cast<ICombatInterface>(InActor))
	{
		return PawnCombatInterface->GetCombatComponent();
	}
	return nullptr;
}

UCombatComponent* UAbilitySystemLibrary::BP_GetCombatComponentFromActor(AActor* InActor, EValidType& OutValidType)
{
	UCombatComponent* CombatComponent = NativeGetCombatComponentFromActor(InActor);
	OutValidType = CombatComponent? EValidType::Valid : EValidType::Invalid;
	return CombatComponent;
}

float UAbilitySystemLibrary::GetScalableFloatValueAtLevel(const FScalableFloat& ScalableFloat, float InLevel)
{
	return ScalableFloat.GetValueAtLevel(InLevel);
}

FGameplayTag UAbilitySystemLibrary::ComputeHitReactDirection(AActor* InAttacker, AActor* InTarget, float& OutHitDirection)
{
	check(InAttacker && InTarget);
	const FVector TargetForward = InTarget->GetActorForwardVector();
	const FVector TargetAngleNormalized = (InAttacker->GetActorLocation() - InTarget->GetActorLocation()).GetSafeNormal();

	const float DotResult = FVector::DotProduct(TargetForward,TargetAngleNormalized);
	OutHitDirection = UKismetMathLibrary::DegAcos(DotResult);

	const FVector CrossResult = FVector::CrossProduct(TargetForward, TargetAngleNormalized);
	if (CrossResult.Z < 0.f)
	{
		OutHitDirection *= -1.f;
	}

	if (OutHitDirection >= -45.f && OutHitDirection <= 45.f)
	{
		return FGameplayTag::RequestGameplayTag(FName("event.HitReact.front"));
	}
	else if (OutHitDirection<-45.f && OutHitDirection>=-135.f)
	{
		return FGameplayTag::RequestGameplayTag(FName("event.HitReact.left"));
	}
	else if (OutHitDirection<-135.f || OutHitDirection>135.f)
	{
		return FGameplayTag::RequestGameplayTag(FName("event.HitReact.back"));
	}
	else if(OutHitDirection>45.f && OutHitDirection<=135.f)
	{
		return FGameplayTag::RequestGameplayTag(FName("event.HitReact.right"));
	}
	
	return FGameplayTag::RequestGameplayTag(FName("event.HitReact.front"));
}

bool UAbilitySystemLibrary::IsValidBlock(AActor* InAttacker, AActor* InDefender)
{
	check(InAttacker && InDefender);

	const float DotResult = FVector::DotProduct(InAttacker->GetActorForwardVector(), InDefender->GetActorForwardVector());

	const FString DebugString = FString::Printf(TEXT("DotResult: %f %s"), DotResult, DotResult<0.f? TEXT("Valid Block") : TEXT("NO Block"));
	//Debug::Print(DebugString,DotResult<0.f? FColor::Green : FColor::Red);
	return DotResult < 0.f? true : false;;
}

void UAbilitySystemLibrary::CountDown(const UObject* WorldContextObject, float TotalTime, float UpdateInterval,
	float& OutRemainingTime, ECountDownActionInput CountDownInput, ECountDownActionOutput& CountDownOutput,
	FLatentActionInfo LatentInfo)
{
	UWorld* World = nullptr;
	if (GEngine)
	{
		World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	}
	if (!World)
	{
		return;
	}
	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
	FCountDownAction* FoundAction = LatentActionManager.FindExistingAction<FCountDownAction>(LatentInfo.CallbackTarget, LatentInfo.UUID);
	if (CountDownInput == ECountDownActionInput::Start)
	{
		if (!FoundAction)
		{
			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, new FCountDownAction(TotalTime, UpdateInterval, OutRemainingTime, CountDownOutput, LatentInfo));
		}
	}
	if (CountDownInput == ECountDownActionInput::Cancel)
	{
		if (FoundAction)
		{
			FoundAction->CancelAction();
		}
	}
}
