// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPG/Character/RPGCharacter.h"
#include "RPG/Interfaces/StrategyInterface.h"
#include "AttackStrategy.generated.h"

class ARPGCharacter;
struct FPathFollowingResult;
struct FAIRequestID;
/**
 * 
 */
UCLASS()
class RPG_API UAttackStrategy : public UStrategyInterface
{
	GENERATED_BODY()

public:
	UAttackStrategy();
	virtual void Exectue(AEnemy* Enemy);
	virtual void OnMoveCompleted(FAIRequestID,
		const FPathFollowingResult& Result,
		AEnemy* Enemy,
		ARPGCharacter* Player);

private:
	static float PlayerEnemyDistance(AEnemy* Enemy, ARPGCharacter* Player);
};
