// Fill out your copyright notice in the Description page of Project Settings.


#include "ReservedCommand.h"

#include "RPGTests/Component/Entities_Component.h"

void UReservedCommand::Execute()
{
	if (!IsValid())
	{
		Fail();
	}
	Super::Execute();
	
	if (Data.CommandStatus == EEntities_CommandStatus::Active)
	{
		ExecuteReserved();
	}
}

void UReservedCommand::Fail()
{
	Super::Fail();
}

void UReservedCommand::ExecuteReserved()
{
	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Black, FString::Printf(TEXT("Executed Reserved for Command:[%s]"), *Data.Id.ToString()));
	}
	if (IsValid())
	{
		if (UEntities_Component* Entity = UEntities_Component::FindEntityComponent(Data.SourceActor))
		{
			if (!Entity->AssignedCommand(GetId()) && !Entity->HasCompletedCommand(GetId()))
			{
				Entity->ExecuteReservedCommand(this);
			}
		}
	}
}
