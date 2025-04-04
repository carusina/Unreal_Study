// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPG/Interfaces/HitInterface.h"
#include "Enemy.generated.h"

UCLASS()
class RPG_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Hit Interface
	virtual void HitInterface_Implementation(FHitResult HitResult) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitMontage;
	
};
