// Fill out your copyright notice in the Description page of Project Settings.



#include "BattleSubsystem.h"
#include "RPGTests/Component/Entities_Component.h"
#include "RPGTests/Component/Entities_StatComponent.h"
#include "RPGTests/Data/AStaticGameData.h"

void UBattleSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	DamageCalculator = NewObject<UDamageCalculator>(this);
	ReactionHandler = NewObject<UReactionHandler>(this);
	InitiativeHandler = NewObject<UInitiativeHandler>(this);
}

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
	
	InitiativeHandler->BuildQueue(Participants);
	
	ApplyBattleTagToAll(EntityTags::Condition::BattleActive, true);
	
	if (InitiativeHandler->GetQueue().IsEmpty())
	{
		return;
	}
	
	StartTurn(InitiativeHandler->GetCurrent());
}

void UBattleSubsystem::ExitCombat()
{
	if (ResolutionState != EResolutionState::Idle)
	{
		UE_LOG(LogTemp, Warning, TEXT("BattleSubsystem: ExitCombat called mid-resolution."));
	}
	ApplyBattleTagToAll(EntityTags::Condition::BattleActive, false);
	CombatParticipants.Reset();
	InitiativeHandler->Reset();
	DamageCalculator->Reset();
	Stack.Reset();
	SourceCommand = nullptr;
	ResolutionState = EResolutionState::Idle;
	CurrentState = EBattleState::Exploration;
}

void UBattleSubsystem::TryAddParticipant(AActor* Actor)
{
	if (!Actor || !ShouldJoinCombat(Actor)) return;
	
	const bool bAlreadyIn = CombatParticipants.ContainsByPredicate(
		[Actor](const TWeakObjectPtr<AActor>& Participant){ return Participant.Get() == Actor; }
		);
	if (bAlreadyIn) return;
	
	CombatParticipants.Add(Actor);

	InitiativeHandler->InsertParticipant(Actor);
	
}

void UBattleSubsystem::SubmitCommand(UCommandBase* Command)
{
	if (!Command) return;
	
	if (!ValidateCommand(Command))
	{
		Command->Fail();
		return;
	}
	
	if (ResolutionState != EResolutionState::Idle)
	{
		Command->Fail();
		return;
	}
	
	Stack.Reset();
	DamageCalculator->Reset();
	SourceCommand = Command;
	ResolutionState = EResolutionState::Resolving;
	
	Stack.Add(Command);
	ProcessNextOnStack();
}

void UBattleSubsystem::EndTurn()
{
	if (InitiativeHandler->IsCombatOver()) { ExitCombat(); return; }
	
	FTurnParticipant& Next = InitiativeHandler->AdvanceTurn();
	StartTurn(Next);
}

bool UBattleSubsystem::IsActorsTurn(const AActor* Actor) const
{
	const UEntities_StatComponent* StatComp = UEntities_StatComponent::FindEntityStatComponent(Actor);
	if (StatComp->HasStateTag(EntityTags::Condition::BattleTurnActive)) return true;
	return false;
}

void UBattleSubsystem::OnCommandCompleted(const FGuid CommandId, const uint8 bSuccess)
{
	UCommandBase* Completed = SourceCommand;
	
	if (UEntities_Component* Component = UEntities_Component::FindEntityComponent(Completed->Data.SourceActor))
	{
		// TODO: Should put it in history or something
	}
	
	TArray<UCommandBase*> Reactions = ReactionHandler->GatherReactions(Completed, CombatParticipants);
	
	for (int32 i = Reactions.Num() - 1; i >= 0; i--)
	{
		Stack.Add(Reactions[i]);
	}
	
	DamageCalculator->CalculateAndQueue(Completed);
	
	ProcessNextOnStack();
}

void UBattleSubsystem::OnCommandFailed(const FGuid CommandId)
{
	UE_LOG(LogTemp, Warning, TEXT("BattleSubsystem: Command [%s] failed."), *CommandId.ToString());
	
	ProcessNextOnStack();
}

void UBattleSubsystem::ProcessNextOnStack()
{
	if (Stack.IsEmpty())
	{
		FinalizeResolution();
		return;
	}
	
	UCommandBase* Current = Stack.Last();
	Stack.RemoveAt(Stack.Num() - 1);
	
	Current->OnCommandCompleted.AddUObject(this, &UBattleSubsystem::OnCommandCompleted);
	Current->OnCommandFailed.AddUObject(this, &UBattleSubsystem::OnCommandFailed);
	
	Current->Execute();
}

void UBattleSubsystem::FinalizeResolution()
{
	DamageCalculator->CommitAll();
	
	InitiativeHandler->ConsumeAP(SourceCommand);
	
	SourceCommand = nullptr;
	ResolutionState = EResolutionState::Idle;
	if (InitiativeHandler->CurrentParticipantOutOfAP())
	{
		EndTurn();
	}
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

void UBattleSubsystem::StartTurn(FTurnParticipant& Participant)
{
	if (!Participant.IsValid()) { EndTurn(); return; }
	
	
	if (UEntities_StatComponent* StatComp = UEntities_StatComponent::FindEntityStatComponent(Participant.Actor.Get()))
	{
		StatComp->AddStateTag(EntityTags::Condition::BattleTurnActive);
	}
	
	OnTurnChanged.Broadcast(Participant);
}

void UBattleSubsystem::ApplyBattleTagToAll(const FGameplayTag& Tag, bool bAdd)
{

	for (TWeakObjectPtr<AActor> Actor : CombatParticipants)
	{
		if (UEntities_StatComponent* StatComp = UEntities_StatComponent::FindEntityStatComponent(Actor.Get()))
		{
			if (bAdd)
			{
				StatComp->AddStateTag(Tag);
			}
			else
			{
				StatComp->RemoveStateTag(Tag);
			}
		}
	}
}

bool UBattleSubsystem::ValidateCommand(const UCommandBase* Command) const
{
	if (!Command) return false;
	
	if (!InitiativeHandler->IsActorsTurn(Command->Data.SourceActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("BattleSubsystem: Command source is not the current turns actor."))
		return false;
	}
	
	if (!InitiativeHandler->GetCurrent().HasAP())
	{
		UE_LOG(LogTemp, Warning, TEXT("BattleSubsystem: Actor has no AP left."))
		return false;
	}
	
	return true;
}
