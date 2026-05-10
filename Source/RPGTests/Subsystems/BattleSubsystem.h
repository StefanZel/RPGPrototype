// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "BattleHandlers/DamageCalculator.h"
#include "BattleHandlers/InitiativeHandler.h"
#include "BattleHandlers/ReactionHandler.h"
#include "RPGTests/Data/Battle/Battle_DataTypes.h"
#include "RPGTests/Props/CommandResolutionStack.h"
#include "Subsystems/WorldSubsystem.h"
#include "BattleSubsystem.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTurnChanged, const FTurnParticipant&, Participant);
/**
 * 
 */
UCLASS()
class RPGTESTS_API UBattleSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	void NotifyAttack(AActor* Attacker, AActor* Target);
	void NotifySpotted(AActor* Spotter, AActor* Spotted);
	void EnterCombat(const TArray<AActor*>& Participants);
	void ExitCombat();
	void TryAddParticipant(AActor* Actor);
	
	void SubmitCommand(UCommandBase* Command);
	
	void EndTurn();
	
	EBattleState GetState() const { return CurrentState; }
	bool IsActorsTurn(const AActor* Actor) const;
	
	UPROPERTY()
	FOnTurnChanged OnTurnChanged;
	
	
private:
	
	void OnCommandCompleted(const FGuid CommandId, const uint8 bSuccess);
	void OnCommandFailed(const FGuid CommandId);
	void ProcessNextOnStack();
	void FinalizeResolution();
	
	TArray<AActor*> GatherParticipants(const FVector& Origin, float Radius) const;
	bool ShouldJoinCombat(const AActor* Actor) const;
	bool IsAlreadyInCombat() const { return CurrentState != EBattleState::Exploration; }
	void StartTurn(FTurnParticipant& Participant);
	void ApplyBattleTagToAll(const FGameplayTag& Tag, bool bAdd);
	bool ValidateCommand(const UCommandBase* Command) const;
	
	// TODO: Have this in global config
	static constexpr float EngagementRadius = 1500.f;
	
	UPROPERTY()
	EBattleState CurrentState = EBattleState::Exploration;
	
	UPROPERTY()
	EResolutionState ResolutionState = EResolutionState::Idle;
	
	UPROPERTY()
	TArray<UCommandBase*> Stack;
	
	UPROPERTY()
	UCommandBase* SourceCommand = nullptr;
	
	UPROPERTY()
	UDamageCalculator* DamageCalculator = nullptr;
	
	UPROPERTY()
	UReactionHandler* ReactionHandler = nullptr;
	
	UPROPERTY()
	UInitiativeHandler* InitiativeHandler = nullptr;
	
	
	TArray<TWeakObjectPtr<AActor>> CombatParticipants;
};
