// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities_AiControllerCommand.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

AEntities_AiControllerCommand::AEntities_AiControllerCommand(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	ActiveCommand = nullptr;
}

void AEntities_AiControllerCommand::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEntities_AiControllerCommand::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UE_LOG(LogTemp, Warning, TEXT("OnPossess: Controller=%s, Pawn=%s"), *GetName(), *InPawn->GetName());

	InitAiOnGameplay();
}

void AEntities_AiControllerCommand::InitAiOnGameplay()
{
	Super::InitAiOnGameplay();
}

void AEntities_AiControllerCommand::BeginPlay()
{
	Super::BeginPlay();
}

void AEntities_AiControllerCommand::ExecuteCommand(UCommandBase* Command)
{
	if(Command == nullptr) return;
	
	if(ActiveCommand != nullptr)
	{
		if(ActiveCommand->GetId() == Command->GetId()) return;

		CompleteCurrentCommand(EEntities_CommandStatus::Updating);
	}

	ActiveCommand = Command;
	if (ActiveCommand != nullptr)
	{
		if(ActiveCommand->Data.HasNavigation())
		{ 
			ExecuteMovement();
		}
		if (ActiveCommand->Data.HasAbility())
		{
			ExecuteAbility();
		}
	}
}

void AEntities_AiControllerCommand::CompleteCurrentCommand(const EEntities_CommandStatus Status)
{

}

void AEntities_AiControllerCommand::ExecuteMovement()
{
	if(!GetPawn() || !ActiveCommand) return;
	
	if (ActiveCommand->Data.CommandType == EEntities_CommandTypes::Ability)
	{
	
		GetPawn()->SetActorRotation(ActiveCommand->Data.GetRotation());

		return;
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem) return;

	FNavLocation NavLocation;

	if (NavSystem->ProjectPointToNavigation(ActiveCommand->Data.GetLocation(), NavLocation))
	{
		EPathFollowingRequestResult::Type Result = MoveToLocation(NavLocation.Location);
		
		if (Result == EPathFollowingRequestResult::Type::AlreadyAtGoal)
		{
			UE_LOG(LogTemp, Log, TEXT("Already at the destination!"));
		}
		
		if (Result == EPathFollowingRequestResult::Type::Failed)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to move to destination!"));
		}
		
	}
}

void AEntities_AiControllerCommand::ExecuteAbility()
{
	if(!GetPawn() || !ActiveCommand) return;
	
	
}
