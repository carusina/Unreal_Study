// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void URPGAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (RPGCharacter)
	{
		FVector Velocity = RPGCharacter->GetVelocity();
		Velocity.Z = 0;
		Speed = Velocity.Size();

		bIsInAir = RPGCharacter->GetCharacterMovement()->IsFalling();

		FRotator const AimRotation = RPGCharacter->GetBaseAimRotation();
		FRotator const MovementRotation = UKismetMathLibrary::MakeRotFromX(RPGCharacter->GetVelocity());

		Direction = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
	}
}

void URPGAnimInstance::NativeInitializeAnimation()
{
	RPGCharacter = Cast<ARPGCharacter>(TryGetPawnOwner());
}
