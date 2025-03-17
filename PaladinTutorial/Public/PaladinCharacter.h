// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "PaladinCharacter.generated.h"

// Declaration
class USpringArmComponent;
class UCameraComponent;
class UBoxComponent;
class UInputMappingContext;
class UInputAction;
class UAnimMontage;
class USoundCue;

UCLASS()
class PALADINTUTORIAL_API APaladinCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APaladinCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Activate and Deactivate Weapon Boxes
	virtual void ActivateRightWeapon();
	virtual void DeactivateRightWeapon();

	virtual float TakeDamage(float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Input Actions
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* RunAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* BasicAttackAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* HeavyAttackAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* SpinAttackAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* JumpAttackAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* BlockAction;

	// Walk Speed
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed;

	// Run Speed
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	float RunSpeed;
	
	// Movement
	void Move(const FInputActionValue& InputValue);
	void Look(const FInputActionValue& InputValue);
	void Jump();
	void Running();
	void StopRunning();

	// Attacks
	void BasicAttack();
	void HeavyAttack();
	void SpinAttack();
	void JumpAttack();

	// Blocking
	void StartBlocking();
	void StopBlocking();

	// Handle Logic After Player Dies
	UFUNCTION(BlueprintImplementableEvent)
	void DeathOfPlayer();

	void AnimMontagePlay(UAnimMontage* MontageToPlay, FName SectionName = "Default", float PlayRate = 1.0f);

	// Right Weapon Overlap
	UFUNCTION()
	void OnRightWeaponOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
		);

private:
	// Spring Arm Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComponent;

	// Follow Camera Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCameraComponent;

	// Montages
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RightWeaponCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	USoundCue* BodyImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	USoundCue* ShieldImpactSound;

	bool PlayerFacingActor(AActor* FacingActor);

	// AI Perception System
	UAIPerceptionStimuliSourceComponent* StimuliSource;
	void SetupStimulusSource();
};
