// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "PlayerStartBase.generated.h"

/**
 * 
 */
UCLASS()
class RPGTESTS_API APlayerStartBase : public APlayerStart
{
	GENERATED_BODY()

public:

	APlayerStartBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual bool TryClaim(AController* OccupyingController);
	bool IsClaimed() const { return ClaimingController != nullptr; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(Transient)
	TObjectPtr<AController> ClaimingController;

};
