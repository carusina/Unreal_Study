// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAnimInstance.h"

#include "Enemy/Enemy.h"
#include "Kismet/KismetMathLibrary.h"

void UEnemyAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (Enemy == nullptr)
	{
		Enemy = Cast<AEnemy>(TryGetPawnOwner());
	}

	if (Enemy)
	{
		// Get Speed Of Character From Velocity
		FVector Velocity = Enemy->GetVelocity();
		Velocity.Z = 0;
		Speed = Velocity.Size();

		// Get Offset Yaw For Player in Blend Spaces
		FRotator const AimRotation = Enemy->GetBaseAimRotation();
		FRotator const MovementRotation = UKismetMathLibrary::MakeRotFromX(Enemy->GetVelocity());

		Direction = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
	}
}
