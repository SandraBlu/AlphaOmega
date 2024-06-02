// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/XPInfo.h"

int32 UXPInfo::FindLevelForXP(int32 XP) const
{
	int32 Level = 1;
	bool bSearching = true;
	while (bSearching)
	{

		if (LevelUpInfo.Num() - 1 <= Level) return Level;
		if (XP >= LevelUpInfo[Level].LevelUpPointsRequired)
		{
			++Level;
		}
		else
		{
			bSearching = false;
		}
	}
	return Level;
}
