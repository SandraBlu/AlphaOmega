// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UInputMappingContext;
class URGameplayAbility;

UENUM()
enum class EConfirmType : uint8
{
	Yes,
	No
};

UENUM()
enum class EValidType : uint8
{
	Valid,
	Invalid
};

UENUM(BlueprintType)
enum class EFoot : uint8
{
	Left  UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};

UENUM()
enum class ECountDownActionInput : uint8
{
	Start,
	Cancel
};
UENUM()
enum class ECountDownActionOutput : uint8
{
	Updated,
	Completed,
	Cancelled
};

UENUM(BlueprintType)
enum class EGameDifficulty : uint8
{
	Easy,
	Normal,
	Hard,
	VeryHard
};
