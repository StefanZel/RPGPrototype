// Fill out your copyright notice in the Description page of Project Settings.


#include "NavigationCommand.h"
#include "RPGTests/Ai/Entities_AiControllerCommand.h"

UNavigationCommand::UNavigationCommand()
{
}

void UNavigationCommand::Execute()
{
	if (!IsValid())
	{
		Fail();
	}

	Super::Execute();

	if (Data.CommandStatus == EEntities_CommandStatus::Active)
	{
		ExecuteNavigation();
	}
}

void UNavigationCommand::Undo()
{
}

void UNavigationCommand::Redo()
{
}

void UNavigationCommand::Fail()
{
	Super::Fail();
}

void UNavigationCommand::Complete(const UEntities_Component* Entity, const EEntities_CommandStatus StatusUpdate)
{
	Super::Complete(Entity, StatusUpdate);
}

bool UNavigationCommand::IsValid() const
{
	return false;
}

void UNavigationCommand::ExecuteNavigation()
{
	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Black, FString::Printf(TEXT("Executed Navigation for Command:[%s]"), *Data.Id.ToString()));
	}
	if (Data.SourceActor != nullptr)
	{
		if (const APawn* EntityPawn = Cast<APawn>(Data.SourceActor))
		{
			if ( AEntities_AiControllerCommand* AiController = Cast<AEntities_AiControllerCommand>(EntityPawn->GetController()))
			{
				AiController->ExecuteMovement(Data.GetLocation());
			}
		}
	}
}
