// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Abilities_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAbilities_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGTESTS_API IAbilities_Interface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void InitializeAbility(const FPrimaryAssetId& AbilityData) = 0;
	virtual void UpdateAbility(const FVector& Position) = 0;
	virtual TArray<AActor*> GetTargetActorsOnExecute() = 0;
};
