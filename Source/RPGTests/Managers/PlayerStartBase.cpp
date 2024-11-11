// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStartBase.h"

APlayerStartBase::APlayerStartBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

bool APlayerStartBase::TryClaim(AController* OccupyingController)
{
	if (OccupyingController != nullptr && !IsClaimed())
	{
		ClaimingController = OccupyingController;
		return true;
	}
	return false;
}

void APlayerStartBase::BeginPlay()
{
	Super::BeginPlay();
}
