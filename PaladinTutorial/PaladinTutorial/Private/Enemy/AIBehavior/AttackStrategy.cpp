﻿#include "Enemy/AIBehavior/AttackStrategy.h"

#include "PaladinCharacter.h"
#include "Enemy/Enemy.h"
#include "Enemy/EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

UAttackStrategy::UAttackStrategy()
{
}

void UAttackStrategy::Execute(AEnemy* Enemy)
{
	// Move to Player and Attack
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(Enemy->GetWorld(), 0);
	APaladinCharacter* PaladinCharacter = Cast<APaladinCharacter>(Pawn);

	if (PaladinCharacter)
	{
		AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(Enemy->GetController());

		if (EnemyAIController)
		{
			// Set Focus on the  Player
			EnemyAIController->SetFocus(PaladinCharacter); // AI가 PaladinCharacter를 바라보게 설정

			// Check Player and Enemy Distance for Acceptance Range to Attack
			if (PlayerEnemyDistance(Enemy, PaladinCharacter) <= Enemy->GetAcceptanceRange())
			{
				Enemy->MeleeRangeAttack();
			}
			
			EPathFollowingRequestResult::Type MoveResult = EnemyAIController->MoveToActor(PaladinCharacter,
				Enemy->GetAcceptanceRange(), true, true, true, nullptr, true);

			if (MoveResult == EPathFollowingRequestResult::RequestSuccessful)
			{
				UPathFollowingComponent* PathFollowingComponent = EnemyAIController->GetPathFollowingComponent();
				if (PathFollowingComponent)
				{
					PathFollowingComponent->OnRequestFinished.AddUObject(this, &UAttackStrategy::OnMoveCompleted, Enemy, PaladinCharacter);
				}	
			}
		}
	}
}

void UAttackStrategy::OnMoveCompleted(FAIRequestID, const FPathFollowingResult& Result, AEnemy* Enemy, APaladinCharacter* PaladinCharacter)
{
	if (Result.IsSuccess())
	{
		// Make sure in Range to Attack Player
		if (PlayerEnemyDistance(Enemy, PaladinCharacter) <= Enemy->GetAttackRange())
		{
			Enemy->MeleeRangeAttack();
		} 
	}
}

float UAttackStrategy::PlayerEnemyDistance(AEnemy* Enemy, APaladinCharacter* PaladinCharacter)
{
	FVector PaladinPos = PaladinCharacter->GetActorLocation();
	FVector EnemyPos = Enemy->GetActorLocation();
	float Distance = FVector::Dist(EnemyPos, PaladinPos);

	return Distance;
}
