// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/AODamageEffectCalc.h"
#include "Attributes/AOPlayerAttributeSet.h"
#include "AbilitySystemComponent.h"


struct AODamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);

	AODamageStatics()
	{
		// Capture Targets Armor, use health value at moment we apply execution
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAOPlayerAttributeSet, DamageResistance, Target, false);
		// Capture Targets Agility, use health value at moment we apply execution
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAOPlayerAttributeSet, AttackPower, Source, true);
		// Capture Targets Block, use health value at moment we apply execution
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAOPlayerAttributeSet, Damage, Source, true);
 	}
};

static const AODamageStatics& DamageStatics()
{
	static AODamageStatics DmgStatics;
	return DmgStatics;
}

UAODamageEffectCalc::UAODamageEffectCalc()
{
	RelevantAttributesToCapture.Add(DamageStatics().DamageResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().AttackPowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
}


void UAODamageEffectCalc::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecuteParams, OUT FGameplayEffectCustomExecutionOutput& OutExecuteOutput) const
{
	UAbilitySystemComponent* TargetASC = ExecuteParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceASC = ExecuteParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceASC ? SourceASC->GetAvatarActor_Direct() : nullptr;
	AActor* TargetActor = TargetASC ? TargetASC->GetAvatarActor_Direct() : nullptr;

	const FGameplayEffectSpec& Spec = ExecuteParams.GetOwningSpec();

	//Gather tags from source and target that affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = SourceTags;
	EvalParams.TargetTags = TargetTags;

	//Damage Done  = Damage * AttackPower / Armor

	//If Attribute  = 0, treated as 1

	float DamageResistance = 0.f;
	ExecuteParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageResistanceDef, EvalParams, DamageResistance);
	if (DamageResistance == 0.f)
	{
		DamageResistance = 1.f;
	}

	float AttackPower = 0.f;
	ExecuteParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackPowerDef, EvalParams, AttackPower);
	float Damage = 0.f;
	ExecuteParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvalParams, Damage);

	float DamageDone = Damage * AttackPower / DamageResistance;
	if (DamageDone > 0.f)
	{
		OutExecuteOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty, EGameplayModOp::Additive, DamageDone));

		// If we do any damage, then any conditional GEs are allowed to fire (once we return)
		OutExecuteOutput.MarkConditionalGameplayEffectsToTrigger();
	}
}


