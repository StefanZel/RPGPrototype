// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGTests/Interfaces/Entities_Interface.h"
#include "RPGTests/Data/Entities/Entities_DataTypes.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FirstTestCharacter.generated.h"

/*
	For some insane reason, if a blueprint is made using this class its root component will be corrupted.
	This will cause all other components to be detached from Mesh and Capsule, resulting in unexpected behaviour.

*/

UCLASS()
class RPGTESTS_API AFirstTestCharacter : public ACharacter, public IEntities_Interface
{
	GENERATED_BODY()

public:

	AFirstTestCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
	virtual void PostInitializeComponents() override;

	float GetMaxSpeed();

	float MaxMovementSpeed;

protected:

	virtual void BeginPlay() override;
	virtual void CreatePerceptionStimuliSourceComponent();
	virtual void CommandMove(const FEntities_Navigation NavigationData);

	UPROPERTY()
	UCharacterMovementComponent* CharacterMoveComponent;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	UAIPerceptionStimuliSourceComponent* StimuliSourceComponent;



};
