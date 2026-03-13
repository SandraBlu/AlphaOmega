// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumTypes.h"
#include "GameFramework/GameModeBase.h"
#include "OGameModeBase.generated.h"

class UAbilityInfo;
class UCharacterClassInfo;
/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API AOGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	UCharacterClassInfo* CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Ability Info")
	UAbilityInfo* AbilityInfo;

	FORCEINLINE EGameDifficulty GetCurrentGameDifficulty() const { return CurrentGameDifficulty;}

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Settings")
	EGameDifficulty CurrentGameDifficulty;
	
};
