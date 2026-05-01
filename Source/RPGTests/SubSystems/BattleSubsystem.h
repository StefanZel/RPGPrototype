// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"
#include "BattleSubsystem.generated.h"

UENUM()
enum class EBattleState
{
	Exploration,
	InBattle
};

USTRUCT()
struct FTurnParticipant
{
	GENERATED_BODY()
	
	UPROPERTY()
	TWeakObjectPtr<AActor> Actor;
	
	UPROPERTY()
	float Initiative = 0.f;
	
	UPROPERTY()
	int32 ActionPointsRemaining = 0;
	
	UPROPERTY()
	FGameplayTag TeamTag;
	
	bool IsValid() const { return Actor.IsValid(); }
	bool operator > (const FTurnParticipant& Other) const
	{
		return Initiative > Other.Initiative;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTurnChanged, const FTurnParticipant&, Participant);
/**
 * 
 */
UCLASS()
class RPGTESTS_API UBattleSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	void NotifyAttack(AActor* Attacker, AActor* Target);
	void NotifySpotted(AActor* Spotter, AActor* Spotted);
	void EnterCombat(const TArray<AActor*>& Participants);
	void ExitCombat();
	
	void TryAddParticipant(AActor* Actor);
	
	void OnCommandCompleted(const FGuid CommandId, const uint8 bSuccess);
	
	void EndTurn();
	
	EBattleState GetState() const { return CurrentState; }
	bool IsActorsTurn(const AActor* Actor) const;
	
	UPROPERTY()
	FOnTurnChanged OnTurnChanged;
	
	
private:
	void BuildInitiativeQueue(const TArray<AActor*>& Participants);
	TArray<AActor*> GatherParticipants(const FVector& Origin, float Radius) const;
	bool ShouldJoinCombat(const AActor* Actor) const;
	bool IsAlreadyInCombat() const { return CurrentState != EBattleState::Exploration; }
	bool CheckCombatEnd() const;
	float GetInitiativeForActor(const AActor* Actor) const;
	void StartTurn(FTurnParticipant& Participant);
	void AdvanceTurn();
	
	void ApplyBattleTagToAll(const FGameplayTag& Tag, bool bAdd);
	// TODO: Have this in global config
	static constexpr float EngagementRadius = 1500.f;
	
	UPROPERTY()
	EBattleState CurrentState = EBattleState::Exploration;
	
	UPROPERTY()
	TArray<FTurnParticipant> InitiativeQueue;
	
	UPROPERTY()
	int32 CurrentTurnIndex = 0;
	
	TArray<TWeakObjectPtr<AActor>> CombatParticipants;
};
