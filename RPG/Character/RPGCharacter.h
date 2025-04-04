// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "RPGCharacter.generated.h"

// 선언
class USpringArmComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class UBoxComponent;

class UInputMappingContext;
class UInputAction;

class UAnimMontage;

UCLASS()
class RPG_API ARPGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARPGCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Attack
	FTimerHandle ComboTimer;
	int AttackCombo = 0;
	bool IsAttacking = false;

	virtual void ActivateWeapon();
	virtual void DeactivateWeapon();

	// Turning
	bool bCanTurn = true;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Enhanced Input
	UPROPERTY(EditAnywhere, Category = EnhancedInput)
	UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, Category = EnhancedInput)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = EnhancedInput)
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, Category = EnhancedInput)
    UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, Category = EnhancedInput)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = EnhancedInput)
	UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, Category = EnhancedInput)
	UInputAction* AttackAction;

	// Movement
	void Move(const FInputActionValue& InputValue);
	void Look(const FInputActionValue& InputValue);
	void SprintBegin();
	void SprintEnd();
	void Jump();
	void Dodge();
	void DelayedJump();
	void BasicAttack();
	void ResetCombo();

	// Speed
	UPROPERTY(EditAnywhere, Category = Movement)
	float WalkSpeed = 300.f;
	UPROPERTY(EditAnywhere, Category = Movement)
	float SprintSpeed = 600.f;
	
	// Weapon Collision Overlap
	UFUNCTION()
	void OnWeaponOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

private:
	// 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComp;

	// 무기
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponSkeletal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* WeaponCollision;

	// Montage
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* JumpMontage;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DodgeMontage;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* BasicAttackMontage;

	void AnimMontagePlay(UAnimMontage* MontageToPlay, FName SectionName = "Default", float PlayRate = 1.0f);
};
