// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"

#include "PaladinCharacter.h"
#include "Enemy/EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
AEnemy::AEnemy() :
	BaseDamage(5.f),
	Health(100.f),
	MaxHealth(100.f),
	AttackRange(300.f),
	AcceptanceRange(125.f)
{
	// Right Weapon Collision Box
	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Right Weapon Box"));
	RightWeaponCollision->SetupAttachment(GetMesh(), RightWeaponSocketName);
}

void AEnemy::EnterCombat()
{
	CurrentState = EAIState::Attack;
}

void AEnemy::ExitCombat()
{
	bIsWaiting = false;
	CurrentState = EAIState::Patrol;
	if (EnemyAIController != nullptr)
	{
		EnemyAIController->ClearFocus(EAIFocusPriority::Gameplay);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyAIController is Null in ExitCombat"));
	}
}

void AEnemy::ActivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// Can Enemy Patrol
	CurrentState = EAIState::Patrol;
	
	// Setup Enemy Controller
	EnemyAIController = Cast<AEnemyAIController>(GetController());

	// Bind Function to Overlap for Weapon Box
	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnRightWeaponOverlap);

	// Setup Right Weapon Collision
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AEnemy::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;

	APaladinCharacter* Character = Cast<APaladinCharacter>(OtherActor);

	if (Character)
	{
		UGameplayStatics::ApplyDamage(
			Character,
			BaseDamage,
			EnemyAIController,
			this,
			UDamageType::StaticClass());
	}
}

void AEnemy::MeleeAttack()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AttackMontage)
	{
		if (!AnimInstance->Montage_IsPlaying(AttackMontage))
		{
			// Get Number of Montage Section
			int32 const SectionCount = AttackMontage->CompositeSections.Num();

			// Get Random Animation to Play
			// Get Section Index And PlayTime to Use for Timer
			FName const SectionName = GetAttackSectionName(SectionCount);
			int32 const SectionIndex = AttackMontage->GetSectionIndex(SectionName);
			float const SectionLength = AttackMontage->GetSectionLength(SectionIndex);

			// Play Montage Section
			AnimInstance->Montage_Play(AttackMontage);
			AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);

			// Call Reset Melee Attack
			FTimerHandle TimerResetAttack;
			GetWorldTimerManager().SetTimer(TimerResetAttack, this, &AEnemy::ResetMeleeAttack, SectionLength, false);
		}
	}
}

void AEnemy::ResetMeleeAttack()
{
	float RandomChance = FMath::FRand();
	if (RandomChance <= 0.3f)
	{
		CurrentState = EAIState::Strafe;
	}
}

void AEnemy::ResetAttack()
{
	// Reset Enemy State
}

FName AEnemy::GetAttackSectionName(int32 SectionCount)
{
	FName SectionName;

	// Get Random Section Montage
	const int32 Section = FMath::RandRange(1, SectionCount);

	switch (Section)
	{
	case 1:
		SectionName = FName("Attack1");
		break;
	case 2:
		SectionName = FName("Attack2");
		break;
	case 3:
		SectionName = FName("Attack3");
		break;
	default:
		SectionName = FName("Attack1");
	}
	
	return SectionName;
}

void AEnemy::EnemyPatrol()
{
	if (PatrolStrategy.IsValid())
	{
		PatrolStrategy->Execute(this);
	}
	else
	{
		PatrolStrategy = NewObject<UPatrolStrategy>();
		PatrolStrategy->Execute(this);
	}
	bIsWaiting = false;
}

void AEnemy::EnemyAttack()
{
	if (AttackStrategy.IsValid())
	{
		AttackStrategy->Execute(this);
	}
	else
	{
		AttackStrategy = NewObject<UAttackStrategy>();
		AttackStrategy->Execute(this);
	}
	bIsWaiting = false;
}

void AEnemy::EnemyStrafe()
{
	bIsWaiting = false;
	CurrentState = EAIState::Attack;
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	switch (CurrentState)
	{
	case EAIState::Attack:
		if (!bIsWaiting)
		{
			bIsWaiting = true;
			float AttackDelay = FMath::RandRange(0.75f, 2.f);
			FTimerHandle AttackDelayTimer;
			GetWorldTimerManager().SetTimer(AttackDelayTimer, this, &AEnemy::EnemyAttack, AttackDelay, false);
		}
		break;
	case EAIState::Strafe:
		if (StrafeStrategy->HasReachedDestination(this) && !bIsWaiting)
		{
			bIsWaiting = true;
			if (StrafeStrategy.IsValid())
			{
				StrafeStrategy->Execute(this);
			}
			else
			{
				StrafeStrategy = NewObject<UStrafeStrategy>();
				StrafeStrategy->Execute(this);
			}
			float StrafeDelay = FMath::RandRange(1.f, StrafeDelayTime);
			FTimerHandle StrafeDelayTimer;
			GetWorldTimerManager().SetTimer(StrafeDelayTimer, this, &AEnemy::EnemyStrafe, StrafeDelay, false);
		}
		break;
	case EAIState::Patrol:
		if (PatrolStrategy->HasReachedDestination(this) && !bIsWaiting)
		{
			bIsWaiting = true;
			float PatrolDelay = FMath::RandRange(1.0f, 5.0f);
			GetWorldTimerManager().SetTimer(PatrolDelayTimer, this, &AEnemy::EnemyPatrol, PatrolDelay, false);
		}
		break;
	}
}

void AEnemy::HitInterface_Implementation(FHitResult HitResult)
{
	// Impact Sound
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	// Impact Niagara
	const FVector SpawnLocation = GetMesh()->GetBoneLocation(ImpactBoneLocation, EBoneSpaces::WorldSpace);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactNiagara, SpawnLocation, GetActorRotation());
	
	// Hit Montage
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator,
	AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		// Call Blueprint Function to Play Death Montage and Clean Things Up
		EnemyAIController->UnPossess();
		SetActorEnableCollision(false);
		EnemyDeath();
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}
