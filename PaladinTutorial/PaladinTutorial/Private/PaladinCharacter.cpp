// Fill out your copyright notice in the Description page of Project Settings.

#include "PaladinCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "PaladinAnimInstance.h"
#include "Components/BoxComponent.h"
#include "HitInterface.h"
#include "PlayerSaveGame.h"
#include "Enemy/Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Perception/AISense_Sight.h"

// Sets default values
APaladinCharacter::APaladinCharacter() :
	WalkSpeed(300.f),
	RunSpeed(600.f),
	BaseDamage(20.f),
	Health(100.f),
	MaxHealth(100.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Camera Boom
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 400.f;
	SpringArmComponent->bUsePawnControlRotation = true;

	// Create Follow Camera
	FollowCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);	
	FollowCameraComponent->bUsePawnControlRotation = false;

	// Jump Settings
	GetCharacterMovement()->JumpZVelocity = 300.f;
	GetCharacterMovement()->AirControl = 0.1f;

	// Right Weapon Collision Box
	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Right Weapon Box"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("SwordSocket"));

	// Stimulus
	SetupStimulusSource();

	// Motion Warping Component
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("Motion Warping Component"));
}

void APaladinCharacter::SavePlayerData()
{
	UPlayerSaveGame* SaveGameInstance = Cast<UPlayerSaveGame>(UGameplayStatics::CreateSaveGameObject(UPlayerSaveGame::StaticClass()));
	if (SaveGameInstance)
	{
		SaveGameInstance->Health = Health;
		SaveGameInstance->CheckpointLocation = GetActorLocation();

		// Save Created Object to File
		if (!UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("PlayerSaveSlot"), 0)) // Cannot Save Game
		{
			UE_LOG(LogTemp, Warning, TEXT("SaveGameToSlot Failed."));
		}
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Save Game."));
	}
}

void APaladinCharacter::LoadPlayerData()
{
	UPlayerSaveGame* LoadGameInstance = Cast<UPlayerSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSaveSlot"), 0));

	if (LoadGameInstance)
	{
		Health = LoadGameInstance->Health;
		CheckpointLocation = LoadGameInstance->CheckpointLocation;
	}
}

// Called when the game starts or when spawned
void APaladinCharacter::BeginPlay()
{
	Super::BeginPlay();

	LoadPlayerData();

	// Spawn Player at Checkpoint Location
	UWorld* World = GetWorld();
	if (World && CheckpointLocation != FVector::ZeroVector)
	{
		SetActorLocation(CheckpointLocation);
	}

	CurrentState = EPlayerState::Ready;
	
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		// Get Local PLayer Subsystem
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Add Input Context
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}

	// Bind Function to Overlap for Weapon Box
	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &APaladinCharacter::OnRightWeaponOverlap);

	// Setup Right Weapon Collision
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void APaladinCharacter::MotionWarpAttack(float AttackDistance, FName MotionWarpName)
{
	const FVector Start = GetActorLocation();
	const FVector End = Start + GetActorForwardVector() * AttackDistance;
	FHitResult HitResult;

	// Setup Collision Query Params to Ignore the Player
	FCollisionQueryParams TraceParams(FName(TEXT("AttackTrace")), true, this);
	TraceParams.bReturnPhysicalMaterial = false;

	// Perform Line Trace
	ECollisionChannel ECC_Channel = ECC_Pawn;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Channel, TraceParams))
	{
		AEnemy* Enemy = Cast<AEnemy>(HitResult.GetActor());
		if (Enemy && MotionWarpingComponent)
		{
			if (HitResult.bBlockingHit && HitResult.GetActor() == Enemy)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Hit Enemy"));
				MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(MotionWarpName, HitResult.Location);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy is Null Or Motion Warping Component is Null"));
		}
		//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0 ,1);
	}
}

void APaladinCharacter::ResetWarpAttack()
{
	MotionWarpingComponent->RemoveAllWarpTargets();
}

void APaladinCharacter::Move(const FInputActionValue& InputValue)
{
	FVector2d InputVector = InputValue.Get<FVector2d>();

	if (IsValid(Controller))
	{
		// Get Forward Direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Rotation Matrix
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Add Movement Input
		AddMovementInput(ForwardDirection, InputVector.Y);
		AddMovementInput(RightDirection, InputVector.X);
	}
}

void APaladinCharacter::Look(const FInputActionValue& InputValue)
{
	FVector2d InputVector = InputValue.Get<FVector2d>();

	if (IsValid(Controller))
	{
		AddControllerYawInput(InputVector.X);
		AddControllerPitchInput(InputVector.Y);
	}
}

void APaladinCharacter::Jump()
{
	// Call Parent Class Jump Function
	Super::Jump();

	if (GetCharacterMovement()->IsMovingOnGround())
	{
		// Get Current Forward Velocity
		FVector ForwardVelocity = GetVelocity();
		ForwardVelocity.Z = 0;

		// Define Jump Vertical Velocity
		float JumpVerticalVelocity = GetCharacterMovement()->JumpZVelocity;

		// Combine Current Forward Velocity With Jump Vertical Velocity
		FVector JumpVelocity = ForwardVelocity + FVector(0, 0, JumpVerticalVelocity);

		// Launch Character With The Combined Velocity
		LaunchCharacter(JumpVelocity, true, true);
	}
}

void APaladinCharacter::Running()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void APaladinCharacter::StopRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void APaladinCharacter::BasicAttack()
{
	AnimMontagePlay(AttackMontage, FName("Attack1"));
}

void APaladinCharacter::HeavyAttack()
{
	AnimMontagePlay(AttackMontage, FName("Attack2"));
}

void APaladinCharacter::SpinAttack()
{
	AnimMontagePlay(AttackMontage, FName("Attack3"));
}

void APaladinCharacter::JumpAttack()
{
	MotionWarpAttack(1000.f, "Attack4");
	AnimMontagePlay(AttackMontage, FName("Attack4"));
}

void APaladinCharacter::DodgeFwd()
{
	CurrentState = EPlayerState::BlockDodge;
	AnimMontagePlay(DodgeMontage, FName("DodgeFwd"));
	GetWorldTimerManager().SetTimer(TimerDodgeRoll, this, &APaladinCharacter::RestDodgeRoll, 0.5f);
}

void APaladinCharacter::DodgeBwd()
{
	CurrentState = EPlayerState::BlockDodge;
	AnimMontagePlay(DodgeMontage, FName("DodgeBwd"));
	GetWorldTimerManager().SetTimer(TimerDodgeRoll, this, &APaladinCharacter::RestDodgeRoll, 0.5f);
}

void APaladinCharacter::DodgeLeft()
{
	CurrentState = EPlayerState::BlockDodge;
	AnimMontagePlay(DodgeMontage, FName("DodgeLeft"));
	GetWorldTimerManager().SetTimer(TimerDodgeRoll, this, &APaladinCharacter::RestDodgeRoll, 0.5f);
}

void APaladinCharacter::DodgeRight()
{
	CurrentState = EPlayerState::BlockDodge;
	AnimMontagePlay(DodgeMontage, FName("DodgeRight"));
	GetWorldTimerManager().SetTimer(TimerDodgeRoll, this, &APaladinCharacter::RestDodgeRoll, 0.5f);
}

void APaladinCharacter::StartBlocking()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("Start Blocking"));
	
	UPaladinAnimInstance* AnimInstance = Cast<UPaladinAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		CurrentState = EPlayerState::BlockDodge;
		GetCharacterMovement()->DisableMovement();
		AnimInstance->SetIsBlocking(true);
	}
}

void APaladinCharacter::StopBlocking()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("Stop Blocking"));

	UPaladinAnimInstance* AnimInstance = Cast<UPaladinAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		CurrentState = EPlayerState::Ready;
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		AnimInstance->SetIsBlocking(false);
	}
}

void APaladinCharacter::RestDodgeRoll()
{
	CurrentState = EPlayerState::Ready;
}

void APaladinCharacter::AnimMontagePlay(UAnimMontage* MontageToPlay, FName SectionName, float PlayRate)
{
	UPaladinAnimInstance* AnimInstance = Cast<UPaladinAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance && MontageToPlay)
	{
		// Check to see If Montage is Playing
		if (!AnimInstance->Montage_IsPlaying(MontageToPlay))
		{
			PlayAnimMontage(MontageToPlay, PlayRate, SectionName);
		}

		if (MontageToPlay == AttackMontage && SectionName == "Attack4")
		{
			FTimerHandle WarpTimer;
			GetWorldTimerManager().SetTimer(WarpTimer, this, &APaladinCharacter::ResetWarpAttack, 2.f, false);
		}
	}
}

void APaladinCharacter::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(SweepResult.GetActor()) && SweepResult.GetActor() != this)
	{
		IHitInterface* HitInterface = Cast<IHitInterface>(SweepResult.GetActor());
		if (HitInterface)
		{
			HitInterface->HitInterface_Implementation(SweepResult);
		}
		
		// Apply Damage to Enemy
		UGameplayStatics::ApplyDamage(
			SweepResult.GetActor(),
			BaseDamage,
			GetController(),
			this,
			UDamageType::StaticClass());
	}
}

bool APaladinCharacter::PlayerFacingActor(AActor* FacingActor)
{
	FVector PlayerDirection = GetActorForwardVector();
	FVector ActorDirection = FacingActor->GetActorLocation() - GetActorLocation().GetSafeNormal();

	float DotProduct = FVector::DotProduct(PlayerDirection, ActorDirection);

	if (DotProduct > 0)
	{
		return true;
	}

	return false;
}

void APaladinCharacter::SetupStimulusSource()
{
	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus Source"));
	if (StimuliSource)
	{
		StimuliSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
		StimuliSource->RegisterWithPerceptionSystem();
	}
}

// Called every frame
void APaladinCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APaladinCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Movement Actions
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APaladinCharacter::Move);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &APaladinCharacter::Look);
		Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APaladinCharacter::Jump);
		Input->BindAction(RunAction, ETriggerEvent::Triggered, this, &APaladinCharacter::Running);
		Input->BindAction(RunAction, ETriggerEvent::Completed, this, &APaladinCharacter::StopRunning);

		// Dodge Actions
		Input->BindAction(DodgeFwdAction, ETriggerEvent::Triggered, this, &APaladinCharacter::DodgeFwd);
		Input->BindAction(DodgeBwdAction, ETriggerEvent::Triggered, this, &APaladinCharacter::DodgeBwd);
		Input->BindAction(DodgeLeftAction, ETriggerEvent::Triggered, this, &APaladinCharacter::DodgeLeft);
		Input->BindAction(DodgeRightAction, ETriggerEvent::Triggered, this, &APaladinCharacter::DodgeRight);

		// Blocking Actions
		Input->BindAction(BlockAction, ETriggerEvent::Triggered, this, &APaladinCharacter::StartBlocking);
		Input->BindAction(BlockAction, ETriggerEvent::Completed, this, &APaladinCharacter::StopBlocking);

		// Attack Actions
		Input->BindAction(BasicAttackAction, ETriggerEvent::Completed, this, &APaladinCharacter::BasicAttack);
		Input->BindAction(HeavyAttackAction, ETriggerEvent::Triggered, this, &APaladinCharacter::HeavyAttack);
		Input->BindAction(SpinAttackAction, ETriggerEvent::Completed, this, &APaladinCharacter::SpinAttack);
		Input->BindAction(JumpAttackAction, ETriggerEvent::Completed, this, &APaladinCharacter::JumpAttack);
	}
}

void APaladinCharacter::ActivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void APaladinCharacter::DeactivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

float APaladinCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	if (CurrentState != EPlayerState::BlockDodge)
	{
		if (Health - DamageAmount <= 0.f)
		{
			Health = 0.f;
			// Play Death Event
			DeathOfPlayer();
		}
		else
		{
			Health -= DamageAmount;
			if (BodyImpactSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, BodyImpactSound, GetActorLocation());
			}
		}
	}
	else // Is Blocking True
	{
		// Check If Player is Facing Enemy - Run Dot Product Logic
		if (PlayerFacingActor(DamageCauser))
		{
			UPaladinAnimInstance* AnimInstance = Cast<UPaladinAnimInstance>(GetMesh()->GetAnimInstance());
			
			if (ShieldImpactSound && AnimInstance->GetIsBlocking())
			{
				UGameplayStatics::PlaySoundAtLocation(this, ShieldImpactSound, GetActorLocation());
			}
		}
		else
		{
			if (BodyImpactSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, BodyImpactSound, GetActorLocation());
			}
			Health -= DamageAmount;
		}
	}
	
	return DamageAmount;
}

