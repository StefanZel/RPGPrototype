// Fill out your copyright notice in the Description page of Project Settings.


#include "NavigationCommand.h"
#include "RPGTests/Ai/Entities_AiControllerCommand.h"
#include "RPGTests/Component/Entities_Component.h"

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
	return Super::IsValid();
}

void UNavigationCommand::ExecuteNavigation()
{
	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Black, FString::Printf(TEXT("Executed Navigation for Command:[%s]"), *Data.Id.ToString()));
	}
	if (IsValid())
	{
		if (UEntities_Component* Entity = UEntities_Component::FindEntityComponent(Data.SourceActor))
		{
			if (!Entity->AssignedCommand(GetId()) && !Entity->HasCompletedCommand(GetId()))
			{
				Entity->ExecuteNavigationCommand(this);
			}
		}
	}
}
