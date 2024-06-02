// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ElectricMissile.h"

FString UElectricMissile::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float EnergyCost = FMath::Abs(GetEnergyCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
		//Title
		"<Title>Electric Missile</>\n\n"
		//Details
		"<Small>Level: </><Level>%d </>\n"
		"<Small>EnergyCost: </><EnergyCost>%.1f </>\n"
		"<Small>Cooldown: </><Cooldown>%.1f </>\n\n"
		//Description
		"<Default>Launches electric current dealing: </> <Damage>%d</> <Default>damage</>\n\n"),
		//Values
		Level, EnergyCost, Cooldown, ScaledDamage);
	}
	else
	{
		return FString::Printf(TEXT(
			//Title
			"<Title>Electric Missile</>\n\n"
			//Details
			"<Small>Level: </><Level>%d </>\n"
			"<Small>EnergyCost: </><EnergyCost>%.1f </>\n"
			"<Small>Cooldown: </><Cooldown>%.1f </>\n\n"
			//Description
			"<Default>Launches %d electric current dealing: </> <Damage>%d</> <Default>damage, with a chance to stun.</>\n\n"),
			//Values
			Level, EnergyCost, Cooldown, FMath::Min(Level,  NumMissiles), ScaledDamage);
	}
}

FString UElectricMissile::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float EnergyCost = FMath::Abs(GetEnergyCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
		//Title
		"<Title>Next Level</>\n\n"
		//Details
		"<Small>Level: </><Level>%d </>\n"
		"<Small>EnergyCost: </><EnergyCost>%.1f </>\n"
		"<Small>Cooldown: </><Cooldown>%.1f </>\n\n"
		//Description
		"<Default>Launches %d electric current dealing: </> <Damage>%d</> <Default>damage, with a chance to stun.</>\n\n"),
		//Values
		Level, EnergyCost, Cooldown, FMath::Min(Level, NumMissiles), ScaledDamage);
}
