// Fill out your copyright notice in the Description page of Project Settings.


#include "InitiativeHandler.h"

#include "RPGTests/Commands/CommandBase.h"
#include "RPGTests/Component/Entities_StatComponent.h"

void UInitiativeHandler::BuildQueue(const TArray<AActor*>& Participants)
{
	Queue.Reset();
	
	for (AActor* Actor: Participants)
	{
		if (!Actor) continue;
		
		FTurnParticipant Entry;
		Entry.Actor = Actor;
		Entry.Initiative = GetInitiativeForActor(Actor);
		Entry.MaxActionPoints = GetActionPointsForActor(Actor);
		Entry.ResetAP();
		Queue.Add(Entry);
	}
	
	Queue.Sort([](const FTurnParticipant& A, const FTurnParticipant& B)
	{
		return A.Initiative > B.Initiative;
	});
	
	CurrentIndex = 0;
}

void UInitiativeHandler::InsertParticipant(AActor* Actor)
{
	if (!Actor) return;
	const bool bAlreadyIn = Queue.ContainsByPredicate(
		[Actor](const FTurnParticipant& Participant){ return Participant.Actor.Get() == Actor; }
		);
	if (bAlreadyIn) return;
	
	FTurnParticipant Entry;
	Entry.Actor = Actor;
	Entry.Initiative = GetInitiativeForActor(Actor);
	Entry.MaxActionPoints = GetActionPointsForActor(Actor);
	Entry.ResetAP();
	
	
	int32 InsertIndex = 0;
	for (; InsertIndex < Queue.Num(); InsertIndex++)
	{
		if (Entry.Initiative > Queue[InsertIndex].Initiative)
			break;
	}
	
	Queue.Insert(Entry, InsertIndex);
	
	if (InsertIndex <= CurrentIndex)
	{
		CurrentIndex++;
	}
}

void UInitiativeHandler::RemoveParticipant(AActor* Actor)
{
	const int32 Index = Queue.IndexOfByPredicate(
		[Actor](const FTurnParticipant& Participant) { return Participant.Actor.Get() == Actor;});
	
	if (Index == INDEX_NONE) return;
	
	Queue.RemoveAt(Index);
	
	if (Index < CurrentIndex)
	{
		CurrentIndex--;
	}
	CurrentIndex = FMath::Clamp(CurrentIndex, 0 , FMath::Max(0, Queue.Num() - 1));
}

FTurnParticipant& UInitiativeHandler::AdvanceTurn()
{
	// TODO: Test this. Problem can probably appear when Actor dies during its turn... 
	Queue.RemoveAll([](const FTurnParticipant& Participant)
	{
		return !Participant.IsValid();
	});
	
	CurrentIndex = (CurrentIndex + 1) % Queue.Num();
	Queue[CurrentIndex].ResetAP();
	return Queue[CurrentIndex];
}

void UInitiativeHandler::ResetCurrentAP()
{
	Queue[CurrentIndex].ResetAP();
}

void UInitiativeHandler::ConsumeAP(const UCommandBase* Command)
{
	if (!Command) return;
	
	FTurnParticipant* Participant = Queue.FindByPredicate(
		[Command](const FTurnParticipant& P)
		{
			return P.Actor.Get() == Command->Data.SourceActor;
		});
	// TODO: Handle AP cost properly
	if (Participant)
	{
		Participant->ConsumeAP(1);
	}
	
}

bool UInitiativeHandler::CurrentParticipantOutOfAP() const
{
	return !Queue[CurrentIndex].HasAP();
}

bool UInitiativeHandler::IsActorsTurn(const AActor* Actor) const
{
	if (Queue.IsEmpty()) return false;
	return Queue[CurrentIndex].Actor.Get() == Actor;
}

bool UInitiativeHandler::IsCombatOver() const
{
	
	return Queue.IsEmpty();
}

void UInitiativeHandler::Reset()
{
	Queue.Reset();
	CurrentIndex = 0;
}

float UInitiativeHandler::GetInitiativeForActor(const AActor* Actor) const
{
	if (UEntities_StatComponent* StatComp = UEntities_StatComponent::FindEntityStatComponent(Actor))
	{
		return StatComp->GetStatValue(EntityTags::Stat::Secondaries::Initiative);
	}
	return 0.f;
}

int32 UInitiativeHandler::GetActionPointsForActor(const AActor* Actor) const
{
	if (UEntities_StatComponent* StatComp = UEntities_StatComponent::FindEntityStatComponent(Actor))
	{
		return StatComp->GetResourceMax(EntityTags::Stat::Resources::ActionPoints);
	}
	return 0;
}
