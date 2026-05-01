// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityCommand.h"
#include "RPGTests/Component/Entities_Component.h"

void UAbilityCommand::Execute()
{
	if (!IsValid())
	{
		Fail();
	}
	Super::Execute();
	if (Data.CommandStatus == EEntities_CommandStatus::Active)
	{
		ExecuteAbility();
	}
}

void UAbilityCommand::Undo()
{
	Super::Undo();
}

void UAbilityCommand::Redo()
{
	Super::Redo();
}

void UAbilityCommand::Fail()
{
	Super::Fail();
}

void UAbilityCommand::ExecuteAbility()
{
	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Black, FString::Printf(TEXT("Executed Ability for Command:[%s]"), *Data.Id.ToString()));
	}
	if (IsValid())
	{
		if (UEntities_Component* Entity = UEntities_Component::FindEntityComponent(Data.SourceActor))
		{
			if (!Entity->AssignedCommand(GetId()) && !Entity->HasCompletedCommand(GetId()))
			{
				Entity->ExecuteAbilityCommand(this);
			}
		}
	}
}


