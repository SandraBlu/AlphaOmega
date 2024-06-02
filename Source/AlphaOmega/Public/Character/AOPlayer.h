// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AOCharacter.h"
#include "Character/CharacterStates.h"
#include "Interfaces/PlayerInterface.h"
#include "AOPlayer.generated.h"


class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UAnimMontage;
class UAOInventoryComponent;
class UInputDataAsset;
class UNiagaraComponent;

/**
 * 
 */
UCLASS()
class ALPHAOMEGA_API AAOPlayer : public AAOCharacter, public IPlayerInterface
{
	GENERATED_BODY()

public:

	AAOPlayer();

	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class UAOEquipmentComponent* Gear;
	
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DrawAnim;

	UFUNCTION(BlueprintCallable)
	bool CanAttack();
	//Set Attack States/Called In Attack Anim Montage-Use PrimaryAttack to set ATtack

	UFUNCTION(BlueprintCallable)
	void AttackEnd();
	
	bool CanDisarm();
	bool CanDraw();
	UFUNCTION(BlueprintCallable)
	void Disarm();
	UFUNCTION(BlueprintCallable)
	void Draw();
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled);

	//Player Interface
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& CombatSocketTag) override;
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetXP_Implementation() const override;
	virtual int32 FindLevelForXP_Implementation(int32 InXP) const override;
	virtual int32 GetAttributePointsReward_Implementation(int32 Level) const override;
	virtual int32 GetAbilityPointsReward_Implementation(int32 Level) const override;
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel) override;
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints) override;
	virtual void AddToAbilityPoints_Implementation(int32 InAbilityPoints) override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual int32 GetAbilityPoints_Implementation() const override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* LevelUpFX;

protected:

	virtual void InitAbilityActorInfo() override;

	//CombatInterface
	virtual int32 GetCharacterLevel_Implementation() override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* FollowCam;

	EEquipState EquipState = EEquipState::EES_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Idle;

public:

	UFUNCTION(BlueprintPure)
	FORCEINLINE EEquipState GetEquipState() const { return EquipState; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE EActionState GetActionState() const { return ActionState; }

protected:

//Input 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* PlayerMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* DrawWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void DrawWeapon(const FInputActionValue& Value);
	void PrimaryAttack(const FInputActionValue& Value);

public:	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	//Custom Input
	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputDataAsset* InputConfig;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

};
