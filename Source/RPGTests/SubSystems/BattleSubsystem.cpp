// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleSubsystem.h"

#include "RPGTests/Component/Entities_Component.h"
#include "RPGTests/Component/Entities_StatComponent.h"
#include "RPGTests/Data/AStaticGameData.h"

void UBattleSubsystem::NotifyAttack(AActor* Attacker, AActor* Target)
{
	if (!Attacker || !Target) return;
	if (IsAlreadyInCombat())
	{
		TryAddParticipant(Attacker);
		TryAddParticipant(Target);
		return;
	}
	
	const FVector Origin = (Attacker->GetActorLocation() + Target->GetActorLocation()) * 0.5f;
	TArray<AActor*> Participants = GatherParticipants(Origin, EngagementRadius);
	
	Participants.AddUnique(Attacker);
	Participants.AddUnique(Target);
	
	EnterCombat(Participants);
}

void UBattleSubsystem::NotifySpotted(AActor* Spotter, AActor* Spotted)
{
	if (!Spotter || !Spotted) return;
	if (IsAlreadyInCombat())
	{
		TryAddParticipant(Spotter);
		TryAddParticipant(Spotted);
		return;
	}
	
	const FVector Origin = Spotter->GetActorLocation();
	TArray<AActor*> Participants = GatherParticipants(Origin, EngagementRadius);
	
	Participants.AddUnique(Spotter);
	Participants.AddUnique(Spotted);
	
	EnterCombat(Participants);
}

void UBattleSubsystem::EnterCombat(const TArray<AActor*>& Participants)
{
	CurrentState = EBattleState::InBattle;
	CombatParticipants.Reset();
	
	for (AActor* Actor : Participants)
	{
		CombatParticipants.Add(Actor);
	}
	
	BuildInitiativeQueue(Participants);
	
	ApplyBattleTagToAll(EntityTags::Condition::BattleActive, true);
	
	StartTurn(InitiativeQueue[0]);
}

void UBattleSubsystem::ExitCombat()
{
}

void UBattleSubsystem::TryAddParticipant(AActor* Actor)
{
	if (!Actor || !ShouldJoinCombat(Actor)) return;
	
	const bool bAlreadyIn = CombatParticipants.ContainsByPredicate(
		[Actor](const TWeakObjectPtr<AActor>& Participant){ return Participant.Get() == Actor; }
		);
	if (bAlreadyIn) return;
	
	CombatParticipants.Add(Actor);
	
	FTurnParticipant NewParticipant;
	NewParticipant.Actor = Actor;
	NewParticipant.Initiative = GetInitiativeForActor(Actor);
	NewParticipant.ActionPointsRemaining = 2;
	
	int32 InsertIndex = 0;
	for (; InsertIndex < InitiativeQueue.Num(); InsertIndex++)
	{
		if (NewParticipant.Initiative > InitiativeQueue[InsertIndex].Initiative)
			break;
	}
	InitiativeQueue.Insert(NewParticipant, InsertIndex);
	
}

void UBattleSubsystem::OnCommandCompleted(const FGuid CommandId, const uint8 bSuccess)
{
	FTurnParticipant& Current = InitiativeQueue[CurrentTurnIndex];
	Current.ActionPointsRemaining--;
	
	if (Current.ActionPointsRemaining <= 0)
	{
		EndTurn();
	}
}

void UBattleSubsystem::EndTurn()
{
	if (CheckCombatEnd()) { ExitCombat(); return; }
	
	do {
		CurrentTurnIndex = (CurrentTurnIndex + 1) % InitiativeQueue.Num();
	} while (!InitiativeQueue[CurrentTurnIndex].IsValid());
	
	StartTurn(InitiativeQueue[CurrentTurnIndex]);
	
}

bool UBattleSubsystem::IsActorsTurn(const AActor* Actor) const
{
	const UEntities_StatComponent* StatComp = UEntities_StatComponent::FindEntityStatComponent(Actor);
	if (StatComp->HasStateTag(EntityTags::Condition::BattleTurnActive)) return true;
	return false;
}

void UBattleSubsystem::BuildInitiativeQueue(const TArray<AActor*>& Participants)
{
}

TArray<AActor*> UBattleSubsystem::GatherParticipants(const FVector& Origin, float Radius) const
{
	TArray<AActor*> Result;
	if (!GetWorld()) return Result;
	
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params;
	Params.bTraceComplex = false;
	
	GetWorld()->OverlapMultiByChannel(
		Overlaps,
		Origin,
		FQuat::Identity,
		TRACE_CHANNEL_ENTITY,
		FCollisionShape::MakeSphere(Radius),
		Params
		);
	
	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Actor = Overlap.GetActor();
		if (ShouldJoinCombat(Actor))
		{
			Result.AddUnique(Actor);
		}
	}
	
	return Result;
}

bool UBattleSubsystem::ShouldJoinCombat(const AActor* Actor) const
{
	const UEntities_Component* EntityComp = UEntities_Component::FindEntityComponent(Actor);
	if (!EntityComp) return false;
	
	// TODO: This should do with tags not this enum
	const EEntities_AvailableTypes Type = EntityComp->GetTypes();
	if (Type == EEntities_AvailableTypes::None || Type == EEntities_AvailableTypes::Resource)
	{
		return false;
	}
	
	return true;
}

bool UBattleSubsystem::CheckCombatEnd() const
{
	// TODO: Change this asap
	if (CombatParticipants.Num() == 0)
	{
		return true;
	}
	return false;
}

float UBattleSubsystem::GetInitiativeForActor(const AActor* Actor) const
{
	return 0.f;
}

void UBattleSubsystem::StartTurn(FTurnParticipant& Participant)
{
	if (!Participant.IsValid()) { EndTurn(); return; }
	
	CurrentTurnIndex = InitiativeQueue.IndexOfByPredicate([&](const FTurnParticipant& P)
	{
		return P.Actor == Participant.Actor;
	});
	
	if (UEntities_Component* EntityComp = UEntities_Component::FindEntityComponent(Participant.Actor.Get()))
	{
		// TODO: Set tags for turn tags
	}
	
	OnTurnChanged.Broadcast(Participant);
}

void UBattleSubsystem::AdvanceTurn()
{
}

void UBattleSubsystem::ApplyBattleTagToAll(const FGameplayTag& Tag, bool bAdd)
{
}
