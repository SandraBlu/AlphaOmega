// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Data/AbilityInfo.h"

FOAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const FOAbilityInfo& Info : AbilityInfo)
	{
		if (Info.AbilityTag == AbilityTag)
		{
			return Info;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find info for AbilityTag[%s] on AblilityInfo [%s]"), *AbilityTag.ToString(), *GetNameSafe(this));
	}
	return FOAbilityInfo();
}
