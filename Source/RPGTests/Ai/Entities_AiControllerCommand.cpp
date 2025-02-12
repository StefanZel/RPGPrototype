// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities_AiControllerCommand.h"

AEntities_AiControllerCommand::AEntities_AiControllerCommand(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	ActiveCommand = nullptr;
}

void AEntities_AiControllerCommand::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEntities_AiControllerCommand::BeginPlay()
{
	Super::BeginPlay();
}

void AEntities_AiControllerCommand::ExecuteMovement(const FVector Destination)
{
		MoveToLocation(Destination);	
}
