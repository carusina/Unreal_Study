// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackStrategy.h"

#include "AITypes.h"
#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "RPG/Enemy/Enemy.h"

UAttackStrategy::UAttackStrategy()
{
}

void UAttackStrategy::Exectue(AEnemy* Enemy)
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(Enemy->GetWorld(), 0);
	ARPGCharacter* Player = Cast<ARPGCharacter>(Pawn);
	if (Player)
	{
		AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(Enemy->GetController());
		if (EnemyAIController)
		{
			EnemyAIController->SetFocus(Player);

			if (PlayerEnemyDistance(Enemy, Player) > 0)
			{
				// 공격
			}

			EPathFollowingRequestResult::Type MoveResult = EnemyAIController->MoveTo(Player);
			if (MoveResult == EPathFollowingRequestResult::RequestSuccessful)
			{
				UPathFollowingComponent* PathFollowingComponent = EnemyAIController->GetPathFollowingComponent();
				if (PathFollowingComponent)
				{
					PathFollowingComponent->OnRequestFinished.AddUObject(this, &UAttackStrategy::OnMoveCompleted, Enemy, Player);
				}
			}
		}
	}
}

void UAttackStrategy::OnMoveCompleted(FAIRequestID, const FPathFollowingResult& Result, AEnemy* Enemy,
	ARPGCharacter* Player)
{
	if (Result.IsSuccess())
	{
		if (PlayerEnemyDistance(Enemy, Player) > 0)
		{
			// 공격
		}
	}
}

float UAttackStrategy::PlayerEnemyDistance(AEnemy* Enemy, ARPGCharacter* Player)
{
	return FVector::Dist(Enemy->GetActorLocation(), Player->GetActorLocation());
}
