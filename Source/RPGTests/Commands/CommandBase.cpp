// Fill out your copyright notice in the Description page of Project Settings.


#include "CommandBase.h"
#include "RPGTests/Component/Entities_Component.h"

UCommandBase::UCommandBase()
{
}

void UCommandBase::Execute()
{
	Activate();
}

void UCommandBase::Fail()
{
	TArray<EEntities_CommandStatus> EntityStates;
	EntitiesStatus.GenerateValueArray(EntityStates);

	for (EEntities_CommandStatus& EntityState : EntityStates)
	{
		EntityState = EEntities_CommandStatus::Failed;
	}

	OnCommandFailed.Broadcast(GetId());
}

void UCommandBase::Complete(const UEntities_Component* Entity, const EEntities_CommandStatus StatusUpdate)
{
	if (EEntities_CommandStatus* StatusPtr = EntitiesStatus.Find(Entity))
	{
		*StatusPtr = EEntities_CommandStatus::Completed;
	}
	if (CheckStatus() == EEntities_CommandStatus::Completed)
	{
		Data.CommandStatus = EEntities_CommandStatus::Completed;
		OnCommandCompleted.Broadcast(GetId(), true);
	}
}

void UCommandBase::SetSelected(AActor* SelectedActor)
{
	Data.SourceActor = SelectedActor;

	AssignCommandData();
}

bool UCommandBase::IsValid() const
{
	return Data.SourceActor != nullptr;
}

void UCommandBase::Activate()
{
	Data.CommandStatus = EEntities_CommandStatus::Active;

	if (IsValid())
	{
		if (UEntities_Component* Entity = UEntities_Component::FindEntityComponent(Data.SourceActor))
		{
			if (!Entity->AssignedCommand(GetId()) && !Entity->HasCompletedCommand(GetId()))
			{
				EntitiesStatus.Add(Entity, Data.CommandStatus);
			}
		}
	}
}

void UCommandBase::AssignCommandData()
{
	FVector SourceLocation = FVector::ZeroVector;
	GetCommandSourceLocation(SourceLocation);

	Data.SourceTransform.SetLocation(SourceLocation);

	if (!Data.TargetTransform.GetLocation().IsNearlyZero())
	{
		Data.SourceTransform.SetRotation(FRotationMatrix::MakeFromX(Data.TargetTransform.GetLocation() - Data.SourceTransform.GetLocation()).ToQuat());
	}
	else
	{
		Data.SourceTransform.SetRotation(FQuat::Identity);
	}
}

void UCommandBase::GetCommandSourceLocation(FVector& SourceLocation) const
{
	if (IsValid())
	{
		SourceLocation = Data.SourceActor->GetActorLocation();
	}
}


EEntities_CommandStatus UCommandBase::CheckStatus() const
{
	TArray<EEntities_CommandStatus> EntityStates;
	EntitiesStatus.GenerateValueArray(EntityStates);

	for (const EEntities_CommandStatus& EntityState : EntityStates)
	{
		if (EntityState == EEntities_CommandStatus::Active)
		{
			return EEntities_CommandStatus::Active;
		}
	}
	// @TODO: Do the same thing for all commands.

	return EEntities_CommandStatus::Completed;
}
