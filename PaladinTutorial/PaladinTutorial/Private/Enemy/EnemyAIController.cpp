// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "PaladinCharacter.h"

AEnemyAIController::AEnemyAIController()
{
	SetupPerceptionSystem();
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	// Add a Small Delay to Ensure the Pawn is Fully initialized
	FTimerHandle TimerPawnInit;
	GetWorld()->GetTimerManager().SetTimer(TimerPawnInit, this, &AEnemyAIController::SetupControlledPawn, 0.1f, false);
}	

void AEnemyAIController::SetupControlledPawn()
{
	AEnemy* Enemy = Cast<AEnemy>(GetPawn());
	if (Enemy != nullptr)
	{
		ControlledEnemy = Enemy;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AEnemyAIController::SetupControlledPawn: No Controlled Pawn Found"));
	}
}

void AEnemyAIController::SetupPerceptionSystem()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	if (SightConfig)
	{
		PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")); // PerceptionComponent는 AI가 감각을 통해 환경을 인식하는 역할
		SetPerceptionComponent(*PerceptionComponent);

		SightConfig->SightRadius = 1500.f; // AI 시야 거리
		SightConfig->LoseSightRadius = SightConfig->SightRadius + 25.f; // AI의 Interest가 떨어지는 범위
		SightConfig->PeripheralVisionAngleDegrees = 120.f; // AI의 시야 각, 양쪽으로 60도씩
		SightConfig->SetMaxAge(5.f); // AI의 Interest가 떨어지는데 걸리는 시간
		SightConfig->AutoSuccessRangeFromLastSeenLocation = 1550.f; // AI가 마지막으로 본 위치에서 1550 유닛 안에 있으면 감지 (시야에 상관없이)
		SightConfig->DetectionByAffiliation.bDetectEnemies = true; // 적 감지
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true; // 아군 감지
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true; // 중립 감지

		GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation()); // AI의 주 감각을 Sight로 설정
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetDetected); // 감지한 대상이 Update될 때 OnTargetDetected 함수 실행
		GetPerceptionComponent()->ConfigureSense(*SightConfig); // AI 시각 감각 활성화
	}
}

void AEnemyAIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	if (APaladinCharacter* const PaladinCharacter = Cast<APaladinCharacter>(Actor))
	{
		if (Stimulus.IsActive())
		{
			if (ControlledEnemy != nullptr)
			{
				ControlledEnemy->EnterCombat();
			}
		}
		else
		{
			if (ControlledEnemy != nullptr)
			{
				ControlledEnemy->ExitCombat();
			}
		}
	}
}
