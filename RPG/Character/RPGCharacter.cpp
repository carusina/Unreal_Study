// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "RPGAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ARPGCharacter::ARPGCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 카메라 시작
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;
	// 카메라 끝

	// 무기 시작
	WeaponSkeletal = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Katana"));
	WeaponSkeletal->SetupAttachment(GetMesh(), "WeaponR");
	// 무기 끝
	
}

// Called when the game starts or when spawned
void ARPGCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}
}

void ARPGCharacter::Move(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(Controller))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, InputVector.Y);
		AddMovementInput(RightDirection, InputVector.X);
	}
}

void ARPGCharacter::Look(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(Controller))
	{
		AddControllerYawInput(InputVector.X);
		AddControllerPitchInput(InputVector.Y);
	}
}

void ARPGCharacter::SprintBegin()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void ARPGCharacter::SprintEnd()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ARPGCharacter::Jump()
{
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		if (JumpMontage)
		{
			PlayAnimMontage(JumpMontage);
		}
		
		FTimerHandle JumpTimerHandle;
		GetWorldTimerManager().SetTimer(JumpTimerHandle, this, &ARPGCharacter::DelayedJump, 0.6f);
	}
}

void ARPGCharacter::DelayedJump()
{
	FVector ForwardVelocity = GetVelocity();
	ForwardVelocity.Z = 0;

	float JumpVerticalVelocity = GetCharacterMovement()->JumpZVelocity;

	FVector JumpVelocity = ForwardVelocity + FVector(0, 0, JumpVerticalVelocity);

	LaunchCharacter(JumpVelocity, true, true);
}

void ARPGCharacter::BasicAttack()
{
	if (AttackCombo >= 3)
	{
		ResetCombo();
	}
	
	switch (AttackCombo)
	{
	case 0:
		AnimMontagePlay(BasicAttackMontage, FName("Attack1"));
		break;
	case 1:
		AnimMontagePlay(BasicAttackMontage, FName("Attack2"));
		break;
	case 2:
		AnimMontagePlay(BasicAttackMontage, FName("Attack3"));
		break;
	default:
		break;
	}
}

void ARPGCharacter::ResetCombo()
{
	AttackCombo = 0;
}

void ARPGCharacter::AnimMontagePlay(UAnimMontage* MontageToPlay, FName SectionName, float PlayRate)
{
	URPGAnimInstance* AnimInstance = Cast<URPGAnimInstance>(GetMesh()->GetAnimInstance());
	
	if (AnimInstance && MontageToPlay)
	{
		if (!AnimInstance->Montage_IsPlaying(MontageToPlay))
		{
			PlayAnimMontage(MontageToPlay, PlayRate, SectionName);

			if (MontageToPlay == BasicAttackMontage)
			{
				AttackCombo++;
				GetWorldTimerManager().ClearTimer(ComboTimer);
				GetWorldTimerManager().SetTimer(ComboTimer, this, &ARPGCharacter::ResetCombo, 1.7f);
			}
		}
	}
}

// Called every frame
void ARPGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARPGCharacter::Move);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARPGCharacter::Look);
		Input->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ARPGCharacter::SprintBegin);
		Input->BindAction(SprintAction, ETriggerEvent::Completed, this, &ARPGCharacter::SprintEnd);
		Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ARPGCharacter::Jump);
		Input->BindAction(AttackAction, ETriggerEvent::Completed, this, &ARPGCharacter::BasicAttack);
	}
}

