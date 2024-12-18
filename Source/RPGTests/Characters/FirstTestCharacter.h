// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGTests/Interfaces/Entities_Interface.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "FirstTestCharacter.generated.h"


UCLASS()
class RPGTESTS_API AFirstTestCharacter : public ACharacter, public IEntities_Interface
{
	GENERATED_BODY()

public:

	AFirstTestCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;
	virtual void CreatePerceptionStimuliSourceComponent();

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	UAIPerceptionStimuliSourceComponent* StimuliSourceComponent;




};
