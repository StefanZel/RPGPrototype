// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ReactionHandler.generated.h"

class UCommandBase;
class UReservedCommand;
struct FTurnParticipant;
/**
 * 
 */
UCLASS()
class RPGTESTS_API UReactionHandler : public UObject
{
	GENERATED_BODY()
public:
	TArray<UCommandBase*> GatherReactions(
		const UCommandBase* ResolvedCommand,
		const TArray<TWeakObjectPtr<AActor>>& CombatParticipants) const;
private:
	bool IsAffectedByCommand(const AActor* Participant, const UCommandBase* Command) const;
	UCommandBase* FindValidReaction(const AActor* Participant, const UCommandBase* Command) const;

	
};
