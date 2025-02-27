// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGTests/Interfaces/Entities_Interface.h"
#include "RPGTests/Data/Entities/Entities_DataTypes.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseEntityCharacter.generated.h"

UCLASS()
class RPGTESTS_API ABaseEntityCharacter : public ACharacter , public IEntities_Interface
{
	GENERATED_BODY()

public:

	ABaseEntityCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;

	virtual void CreatePerceptionStimuliSourceComponent();

	UPROPERTY()
	UCharacterMovementComponent* CharacterMoveComponent;

public:	


private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UAIPerceptionStimuliSourceComponent* StimuliSourceComponent;


};
