// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RPGTests/Data/Battle/Battle_DataTypes.h"
#include "InitiativeHandler.generated.h"

/**
 * 
 */
UCLASS()
class RPGTESTS_API UInitiativeHandler : public UObject
{
	GENERATED_BODY()
public:
	void BuildQueue(const TArray<AActor*>& Participants);
	void InsertParticipant(AActor* Actor);
	void RemoveParticipant(AActor* Actor);
	
	FTurnParticipant& AdvanceTurn();
	
	void ResetCurrentAP();
	
	void ConsumeAP(const UCommandBase* Command);
	
	bool CurrentParticipantOutOfAP() const;
	
	bool IsActorsTurn(const AActor* Actor) const;
	
	const TArray<FTurnParticipant>& GetQueue() const { return Queue; }
	const FTurnParticipant& GetCurrent() const { return Queue[CurrentIndex]; }
	FTurnParticipant& GetCurrent() { return Queue[CurrentIndex]; }
	
	bool IsCombatOver() const;
	void Reset();
private:
	float GetInitiativeForActor(const AActor* Actor) const;
	int32 GetActionPointsForActor(const AActor* Actor) const;
	
	UPROPERTY()
	TArray<FTurnParticipant> Queue;
	
	UPROPERTY()
	int32 CurrentIndex = 0;
};
