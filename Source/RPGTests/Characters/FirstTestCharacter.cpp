// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstTestCharacter.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"


AFirstTestCharacter::AFirstTestCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFirstTestCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFirstTestCharacter::CreatePerceptionStimuliSourceComponent()
{
	StimuliSourceComponent = NewObject<UAIPerceptionStimuliSourceComponent>(this, TEXT("StimuliSourceComponent"));

	if (StimuliSourceComponent != nullptr)
	{
		StimuliSourceComponent->RegisterForSense(UAISense_Sight::StaticClass());
		StimuliSourceComponent->RegisterWithPerceptionSystem();
	}
}

void AFirstTestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



