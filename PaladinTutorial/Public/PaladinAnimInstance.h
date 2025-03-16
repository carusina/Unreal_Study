// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PaladinAnimInstance.generated.h"

// Declarations
class APaladinCharacter;
/**
 * 
 */
UCLASS()
class PALADINTUTORIAL_API UPaladinAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// Used in Animation Blueprints
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	// We can Initialize Variables use them in Blueprints
	virtual void NativeInitializeAnimation() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	APaladinCharacter* PaladinCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsBlocking;

public:
	FORCEINLINE bool GetIsBlocking() const { return bIsBlocking; }
	FORCEINLINE void SetIsBlocking(bool bBlocking) { bIsBlocking = bBlocking; }
};
