// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "RPGAnimInstance.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "RPG/Interfaces/HitInterface.h"

// Sets default values
ARPGCharacter::ARPGCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 카메라
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;

	// 무기
	WeaponSkeletal = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Katana"));
	WeaponSkeletal->SetupAttachment(GetMesh(), "WeaponR");

	WeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponCollision->SetupAttachment(GetMesh(), "WeaponR");

	// StimuliSource
	SetupStimuliSource();
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

	// Weapon Collision Setting
	WeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &ARPGCharacter::OnWeaponOverlap);
	WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollision->SetCollisionObjectType(ECC_WorldDynamic);
	WeaponCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
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

	if (IsValid(Controller) && bCanTurn)
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

void ARPGCharacter::Dodge()
{
	const FVector Forward = GetActorForwardVector();
	const FVector MoveDir = GetVelocity().GetSafeNormal();

	const double CosTheta = FVector::DotProduct(Forward, MoveDir);
	double Theta = FMath::Acos(CosTheta);
	Theta = FMath::RadiansToDegrees(Theta);

	const FVector CrossProduct = FVector::CrossProduct(Forward, MoveDir);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	FName Section("DodgeBwd");
	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("DodgeFwd");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("DodgeLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("DodgeRight");
	}

	AnimMontagePlay(DodgeMontage, Section);
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

void ARPGCharacter::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(SweepResult.GetActor()) && SweepResult.GetActor() != this)
	{
		// Apply Damage
		IHitInterface* HitInterface = Cast<IHitInterface>(SweepResult.GetActor());
		if (HitInterface)
		{
			HitInterface->HitInterface_Implementation(SweepResult);
		}
	}
}

void ARPGCharacter::AnimMontagePlay(UAnimMontage* MontageToPlay, FName SectionName, float PlayRate)
{
	URPGAnimInstance* AnimInstance = Cast<URPGAnimInstance>(GetMesh()->GetAnimInstance());
	
	if (AnimInstance && MontageToPlay)
	{
		if (MontageToPlay == BasicAttackMontage)
		{
			if (!IsAttacking)
			{
				PlayAnimMontage(MontageToPlay, PlayRate, SectionName);
				GetWorldTimerManager().ClearTimer(ComboTimer);
				GetWorldTimerManager().SetTimer(ComboTimer, this, &ARPGCharacter::ResetCombo, 2.3f);
			}
		}
		else if (!AnimInstance->IsAnyMontagePlaying())
		{
			PlayAnimMontage(MontageToPlay, PlayRate, SectionName);
		}
	}
}

void ARPGCharacter::SetupStimuliSource()
{
	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
	if (StimuliSource)
	{
		StimuliSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
		StimuliSource->RegisterWithPerceptionSystem();
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
		Input->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ARPGCharacter::Dodge);
		Input->BindAction(AttackAction, ETriggerEvent::Completed, this, &ARPGCharacter::BasicAttack);
	}
}

void ARPGCharacter::ActivateWeapon()
{
	WeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ARPGCharacter::DeactivateWeapon()
{
	WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

