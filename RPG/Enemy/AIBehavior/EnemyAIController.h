// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class AEnemy;
class UAISenseConfig_Sight;
struct FAIStimulus;
/**
 * 
 */
UCLASS()
class RPG_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

	virtual void BeginPlay() override;

protected:
	UPROPERTY()
	AEnemy* ControlledEnemy;

	UPROPERTY(VisibleAnywhere, Category = AI)
	UAISenseConfig_Sight* SightConfig;

private:
	void SetupControlledPawn();
	void SetupPerceptionSystem();

	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);
};
