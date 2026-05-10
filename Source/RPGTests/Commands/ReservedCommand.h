// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommandBase.h"
#include "ReservedCommand.generated.h"

/**
 * 
 */
UCLASS()
class RPGTESTS_API UReservedCommand : public UCommandBase
{
	GENERATED_BODY()
public:
	virtual void Execute() override;
	virtual void Fail() override;
protected:
	void ExecuteReserved();
};
