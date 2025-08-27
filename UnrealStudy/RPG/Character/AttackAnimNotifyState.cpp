// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackAnimNotifyState.h"

void UAttackAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		Character = Cast<ARPGCharacter>(MeshComp->GetOwner());
		if (Character)
		{
			Character->AttackCombo++;
			Character->bCanTurn = false;
			Character->IsAttacking = true;
			Character->ActivateWeapon();
		}
	}
}

void UAttackAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		if (Character)
		{
			Character->bCanTurn = true;
			Character->IsAttacking = false;
			Character->DeactivateWeapon();
		}
	}
}
