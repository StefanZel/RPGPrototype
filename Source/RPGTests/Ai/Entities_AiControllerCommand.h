// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities_AiControllerMain.h"
#include "RPGTests/Commands/CommandBase.h"
#include "Entities_AiControllerCommand.generated.h"

/**
 * 
 */
UCLASS()
class RPGTESTS_API AEntities_AiControllerCommand : public AEntities_AiControllerMain
{
	GENERATED_BODY()
public:
	AEntities_AiControllerCommand(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void InitAiOnGameplay() override;
	bool HasActiveCommand() const { return ActiveCommand != nullptr; }
	void ExecuteMovement(const FVector& Destination);

	void SetActiveCommand(UCommandBase* NewActiveCommand) { ActiveCommand = NewActiveCommand; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	UCommandBase* ActiveCommand;


};
