// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommandBase.h"
#include "AbilityCommand.generated.h"

/**
 * 
 */
UCLASS()
class RPGTESTS_API UAbilityCommand : public UCommandBase
{
	GENERATED_BODY()
public:
	
	virtual void Execute() override;
	virtual void Undo() override;
	virtual void Redo() override;
	virtual void Fail() override;
	
};
