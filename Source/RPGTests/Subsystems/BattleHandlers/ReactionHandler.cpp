// Fill out your copyright notice in the Description page of Project Settings.


#include "ReactionHandler.h"

#include "RPGTests/Commands/CommandBase.h"
#include "RPGTests/Component/Entities_Component.h"

TArray<UCommandBase*> UReactionHandler::GatherReactions(const UCommandBase* ResolvedCommand,
                                                            const TArray<TWeakObjectPtr<AActor>>& CombatParticipants) const
{
	// TODO: Possibly check if command ResolvedCommand is interaction or something...
	TArray<UCommandBase*> Reactions;
	
	for (const TWeakObjectPtr<AActor>& WeakActor : CombatParticipants)
	{
		if (!WeakActor.IsValid()) continue;
		AActor* Actor = WeakActor.Get();
		
		if (Actor == ResolvedCommand->Data.SourceActor) continue;
		if (!IsAffectedByCommand(Actor, ResolvedCommand)) continue;
		
		UCommandBase* Reaction = FindValidReaction(Actor, ResolvedCommand);
		if (!Reaction) continue;
		
		if (UEntities_Component* Component = UEntities_Component::FindEntityComponent(Actor))
		{
			Component->ConsumeReservedCommand(Reaction);
		}
		Reactions.Add(Reaction);
	}

	return Reactions;
}

bool UReactionHandler::IsAffectedByCommand(const AActor* Participant, const UCommandBase* Command) const
{
	if (Command->Data.TargetActors.Contains(Participant)) return true;
	
	return false;;
}

UCommandBase* UReactionHandler::FindValidReaction(const AActor* Participant, const UCommandBase* Command) const
{
	const UEntities_Component* Component = UEntities_Component::FindEntityComponent(Participant);
	
    if (!Component) return nullptr;
	
	return Component->FindTriggeredReservedCommand(Command->Data);
}

