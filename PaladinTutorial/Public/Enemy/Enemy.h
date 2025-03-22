// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitInterface.h"
#include "AIBehavior/AttackStrategy.h"
#include "AIBehavior/PatrolStrategy.h"
#include "AIBehavior/StrafeStrategy.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "NiagaraFunctionLibrary.h"
#include "Enemy.generated.h"

// List of AI States
UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle	UMETA(DisplayName = "Idle"),
	Patrol	UMETA(DisplayName = "Patrol"),
	Attack	UMETA(DisplayName = "Attack"),
	Combat	UMETA(DisplayName = "Combat"),
	Strafe	UMETA(DisplayName = "Strafe"),
	Dead	UMETA(DisplayName = "Dead"),
};

// Declarations
	class UAnimMontage;
class AEnemyAIController;
class USoundCue;
class UNiagaraSystem;

UCLASS()
class PALADINTUTORIAL_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	// Enter and Exit Combat
	void EnterCombat();
	void ExitCombat();

	void MeleeRangeAttack();
	void ResetMeleeRangeAttack();

	// Spawn Projectile For Enemy Projectile
	void SpawnProjectile();

	// Activate and Deactivate Weapon Box
	virtual void ActivateRightWeapon();
	virtual void DeactivateRightWeapon();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Override Hit Interface
	virtual void HitInterface_Implementation(FHitResult HitResult) override;
	
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;

	// Used for AI State
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	EAIState CurrentState;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Enemy AI Controller
	UPROPERTY()
	AEnemyAIController* EnemyAIController;
	
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
	
	UFUNCTION(BlueprintCallable)
	void ResetAttack();

	FName GetAttackSectionName(int32 SectionCount);

	UFUNCTION(BlueprintImplementableEvent)
	void EnemyDeath();

private:
	// Combat Strategy Logic
	TWeakObjectPtr<UPatrolStrategy> PatrolStrategy;
	TWeakObjectPtr<UAttackStrategy> AttackStrategy;
	TWeakObjectPtr<UStrafeStrategy> StrafeStrategy;

	// This is used in Tick for Patrolling
	bool bIsWaiting;
	FTimerHandle PatrolDelayTimer;
	void EnemyPatrol();
	void EnemyAttack();
	void EnemyStrafe();
	
	// Timer Attack Handle
	FTimerHandle TimerAttack;

	// Right Weapon Collision
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RightWeaponCollision;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FName RightWeaponSocketName = "SwordSocket";

	// Get Blueprint of Projectile. Set this in Enemy Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AEnemyProjectile> ProjectileBP;
	
	// Base Damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float AttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float AcceptanceRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float StrafeDelayTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* ImpactNiagara;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FName ImpactBoneLocation;

public:
	// Getters and Setters
	FORCEINLINE float GetAttackRange() const { return AttackRange; }
	FORCEINLINE float GetAcceptanceRange() const { return AcceptanceRange; }

	FORCEINLINE float SetAttackRange(const float AttackRangeSet)
	{
		AttackRange = AttackRangeSet;
		return AttackRange;
	}
};