// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"

void UEnemyAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (Enemy)
	{
		// Update Variables
	}
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Enemy = Cast<AEnemy>(TryGetPawnOwner());
}
