// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "AIBehavior/EnemyAIController.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// Setup Enemy AI Controller
	EnemyAIController = Cast<AEnemyAIController>(GetController());
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::HitInterface_Implementation(FHitResult HitResult)
{
	if (HitMontage)
	{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "HitMontage");
			PlayAnimMontage(HitMontage);
	}
}

