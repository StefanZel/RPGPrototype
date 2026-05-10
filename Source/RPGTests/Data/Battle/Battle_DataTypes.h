// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameplayTagContainer.h"
#include "Battle_DataTypes.generated.h"

class UCommandBase;

UENUM()
enum class EBattleState
{
	Exploration,
	InBattle
};

UENUM()
enum class EResolutionState : uint8
{
	Idle,
	Resolving,
	AwaitingReactions
};

USTRUCT()
struct FPendingDamage
{
	GENERATED_BODY()
	
	UPROPERTY()
	AActor* Target = nullptr;
	
	UPROPERTY()
	AActor* Source = nullptr;
	
	UPROPERTY()
	float BaseDamage = 0.f;
	
	UPROPERTY()
	float ModifiedDamage = 0.f;
	
	UPROPERTY()
	FGameplayTagContainer PendingStatusEffects;
	
	UPROPERTY()
	bool bNegated = false;
	
	void Negate() { bNegated = true; }
	void Modify(const float Delta) { ModifiedDamage = FMath::Max(0.f, ModifiedDamage + Delta); }
	void AddStatusEffect(const FGameplayTag& Tag) { PendingStatusEffects.AddTag(Tag); }
	
};

USTRUCT()
struct FBattleResolutionState
{
	GENERATED_BODY()
	
	UPROPERTY()
	UCommandBase* SourceCommand = nullptr;
	
	UPROPERTY()
	TArray<FPendingDamage> PendingDamages;
	
	UPROPERTY()
	int32 ReactionDepth = 0;
	
	static constexpr int32 MaxReactionDepth = 0;
	
	bool IsDepthExceeded() const { return ReactionDepth >= MaxReactionDepth; }
	
	void NegateForTarget(const AActor* Target, const float Delta)
	{
		for (FPendingDamage& Entry : PendingDamages)
		{
			if (Entry.Target == Target)
			{
				Entry.ModifiedDamage = FMath::Max(0.f, Entry.ModifiedDamage * Delta);
				return;
			}
		}
	}
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
	int32 MaxActionPoints = 2;
	
	UPROPERTY()
	FGameplayTag TeamTag;
	
	bool IsValid() const { return Actor.IsValid(); }
	bool HasAP() const { return ActionPointsRemaining > 0; }
	void ResetAP() { ActionPointsRemaining = MaxActionPoints; }
	void ConsumeAP(const int32 Cost) { ActionPointsRemaining = FMath::Max(0, ActionPointsRemaining - Cost); }
	bool operator > (const FTurnParticipant& Other) const
	{
		return Initiative > Other.Initiative;
	}
};
