// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommandBase.h"
#include "NavigationCommand.generated.h"

/**
 * 
 */
UCLASS()
class RPGTESTS_API UNavigationCommand : public UCommandBase
{
	GENERATED_BODY()
public:
	UNavigationCommand();

	virtual void Execute() override;
	virtual void Undo() override;
	virtual void Redo() override;
	virtual void Fail() override;
	virtual void Complete(const UEntities_Component* Entity, const EEntities_CommandStatus StatusUpdate) override;


	virtual FEntities_Navigation GetNavigation()  const { return Data.Navigation; };

protected:
	virtual bool IsValid() const override;

	virtual void ExecuteNavigation();
};
