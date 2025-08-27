// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StrategyInterface.generated.h"

class ARPGCharacter;
class AEnemy;
// This class does not need to be modified.
UINTERFACE()
class UStrategyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPG_API IStrategyInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Exectue(AEnemy* Enemy, ARPGCharacter* Player) = 0;
};
