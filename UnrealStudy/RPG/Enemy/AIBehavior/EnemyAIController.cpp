// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"
#include "RPG/Character/RPGCharacter.h"
#include "RPG/Enemy/Enemy.h"

AEnemyAIController::AEnemyAIController()
{
	SetupPerceptionSystem();
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle TimerPawnInit;
	GetWorld()->GetTimerManager().SetTimer(TimerPawnInit, this, &AEnemyAIController::SetupControlledPawn, 0.1f, false);
}

void AEnemyAIController::SetupControlledPawn()
{
	ControlledEnemy = Cast<AEnemy>(GetPawn());
	if (!ControlledEnemy)
	{
		UE_LOG(LogTemp, Error, TEXT("AEnemyAIController::SetupControlledPawn: No Controlled Pawn Found!"));
	}
}

void AEnemyAIController::SetupPerceptionSystem()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	if (SightConfig)
	{
		PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
		SetPerceptionComponent(*PerceptionComponent);

		SightConfig->SightRadius = 1500.0f;
		SightConfig->LoseSightRadius = SightConfig->SightRadius + 25.0f;
		SightConfig->PeripheralVisionAngleDegrees = 120.0f;
		SightConfig->SetMaxAge(5.0f);
		SightConfig->AutoSuccessRangeFromLastSeenLocation = 1500.f;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetDetected);
		GetPerceptionComponent()->ConfigureSense(*SightConfig);
	}
}

void AEnemyAIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	if (ARPGCharacter* const Target = Cast<ARPGCharacter>(Actor))
	{
		if (Stimulus.IsActive())
		{
			if (ControlledEnemy)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("OnTargetDetected"));
				// ControlledEnemy Enter Combat
			}
		}
		else
		{
			if (ControlledEnemy)
			{
				// ControlledEnemy Exit Combat
			}
		}
	}
}
