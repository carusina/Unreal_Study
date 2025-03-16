#include "Enemy/AIBehavior/PatrolStrategy.h"

#include "NavigationData.h"
#include "NavigationSystem.h"
#include "Enemy/Enemy.h"
#include "Enemy/EnemyAIController.h"
#include "Navigation//PathFollowingComponent.h"

UPatrolStrategy::UPatrolStrategy()
{
}

void UPatrolStrategy::Execute(AEnemy* Enemy)
{
	// Add Logic So Enemy Can Patrol
	// Get NavMesh Data
	FNavAgentProperties NavAgentProps;
	const ANavigationData* NavData = UNavigationSystemV1::GetCurrent(Enemy->GetWorld())->GetNavDataForProps(NavAgentProps);

	if (NavData != nullptr)
	{
		// Placeholder for Our Result
		FNavLocation ResultLocation;

		// Get Random Point in Radius
		UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Enemy->GetWorld());
		if (NavSystem && NavSystem->GetRandomReachablePointInRadius(Enemy->GetActorLocation(), 800.f, ResultLocation))
		{
			// Use the AI Controller to Move to the Result Location
			AEnemyAIController* AIController = Cast<AEnemyAIController>(Enemy->GetController());

			// Check if we got Controller and Move to the point
			if (AIController != nullptr)
			{
				AIController->MoveToLocation(ResultLocation.Location);
			}
		}
	}
}

bool UPatrolStrategy::HasReachedDestination(AEnemy* Enemy)
{
	AEnemyAIController* AIController = Cast<AEnemyAIController>(Enemy->GetController());
	if (AIController != nullptr)
	{
		EPathFollowingStatus::Type Status = AIController->GetMoveStatus();
		if (Status == EPathFollowingStatus::Idle || Status == EPathFollowingStatus::Waiting)
		{
			return true;
		}
	}

	return false;
}
