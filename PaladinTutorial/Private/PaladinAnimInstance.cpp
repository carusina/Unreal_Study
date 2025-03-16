// Fill out your copyright notice in the Description page of Project Settings.


#include "PaladinAnimInstance.h"

#include "PaladinCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Acts Like Tick For Animation and Get Speed
void UPaladinAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (PaladinCharacter == nullptr)
	{
		PaladinCharacter = Cast<APaladinCharacter>(TryGetPawnOwner());
	}

	if (PaladinCharacter)
	{
		// Get Speed Of Character From Velocity
		FVector Velocity = PaladinCharacter->GetVelocity();
		Velocity.Z = 0;
		Speed = Velocity.Size();

		bIsInAir = PaladinCharacter->GetCharacterMovement()->IsFalling();

		// Get Offset Yaw For Player in Blend Spaces
		FRotator const AimRotation = PaladinCharacter->GetBaseAimRotation();
		FRotator const MovementRotation = UKismetMathLibrary::MakeRotFromX(PaladinCharacter->GetVelocity());

		Direction = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
	}
}

// Acts Like BeginPlay Create Reference to Character
void UPaladinAnimInstance::NativeInitializeAnimation()
{
	PaladinCharacter = Cast<APaladinCharacter>(TryGetPawnOwner());
}
