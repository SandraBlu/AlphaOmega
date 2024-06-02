// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharClassInfo UCharacterClassInfo::GetClassInfo(ECharacterClass CharacterClass)
{
	return CharClassInfo.FindChecked(CharacterClass);
}
