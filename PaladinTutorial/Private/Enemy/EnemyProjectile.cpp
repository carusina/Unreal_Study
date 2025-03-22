// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyProjectile.h"

#include "PaladinCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AEnemyProjectile::AEnemyProjectile() :
	ProjectileDamage(10.f),
	ProjectileLife(5.f)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SetRootComponent(SphereComponent);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	ProjectileMesh->SetupAttachment(SphereComponent);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed= 3000.f;
}

void AEnemyProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if it hits the Player
	if (OtherActor == nullptr)	return;

	APaladinCharacter* Character = Cast<APaladinCharacter>(OtherActor);
	if (Character)
	{
		UGameplayStatics::ApplyDamage(
			Character,
			ProjectileDamage,
			nullptr,
			this,
			UDamageType::StaticClass()
			);
	}
	else
	{
		GetWorldTimerManager().SetTimer(ProjectileTimer, this, &AEnemyProjectile::DestroyProjectile, ProjectileLife);
	}
}

void AEnemyProjectile::DestroyProjectile()
{
	Destroy();
}

// Called when the game starts or when spawned
void AEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Bind Function to Overlap Events For Collision Boxes
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyProjectile::OnProjectileOverlap);
}

// Called every frame
void AEnemyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

