// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FirstTestCharacter.generated.h"

UCLASS()
class RPGTESTS_API AFirstTestCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AFirstTestCharacter(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

};
