// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Data/AOCharacterInfo.h"

FCharacterDefaultInfo UAOCharacterInfo::GetClassDefaultInfo(ECharacterClass CharacterClass)
{
	return CharClassInfo.FindChecked(CharacterClass);
}
