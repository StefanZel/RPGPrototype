// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RPGTests/Data/Entities/Entities_DataTypes.h"
//#include "RPGTests/Component/Entities_Component.h"
#include "CommandBase.generated.h"

class UEntities_Component;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCommandCompletedDelegate, const FGuid Id, const uint8 Success);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCommandFailedDelegate, const FGuid Id);
/**
 * 
 */
UCLASS(Abstract)
class RPGTESTS_API UCommandBase : public UObject
{
	GENERATED_BODY()
public:
	UCommandBase();

	virtual void Execute();
	virtual void Undo() PURE_VIRTUAL(UCommandBase::Undo, ;);
	virtual void Redo() PURE_VIRTUAL(UCommandBase::Redo, ;);
	virtual void Fail();
	virtual void Complete(const UEntities_Component* Entity, const EEntities_CommandStatus StatusUpdate);

	FOnCommandCompletedDelegate OnCommandCompleted;
	FOnCommandFailedDelegate OnCommandFailed;

	// Gets
	FGuid GetId() const { return Data.Id; }

	// Set
	virtual void SetSelected(AActor* SelectedActor);
	void SetQueued() { Data.CommandStatus = EEntities_CommandStatus::Queued; }


	UPROPERTY()
	FEntities_CommandData Data;

protected:
	virtual bool IsValid() const;
	virtual void Activate();
	virtual void AssignCommandData();
	virtual void GetCommandSourceLocation(FVector& CenterLocation) const;
	virtual EEntities_CommandStatus CheckStatus() const;

	UPROPERTY()
	TMap<UEntities_Component*, EEntities_CommandStatus> EntitiesStatus;
};
