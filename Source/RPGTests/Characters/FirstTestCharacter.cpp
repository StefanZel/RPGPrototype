// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstTestCharacter.h"


AFirstTestCharacter::AFirstTestCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFirstTestCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFirstTestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



