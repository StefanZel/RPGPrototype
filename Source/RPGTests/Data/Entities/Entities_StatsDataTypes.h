// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Entities_Tags.h"
#include "GameplayTagContainer.h"

#include "Entities_StatsDataTypes.generated.h"

UENUM(BlueprintType)
enum class EStatModifierType : uint8
{
	BaseValue,
	FlatAdd,
	Increased,
	Reduced,
	More,
	Less,
	Override,
	Convert
};

UENUM(BlueprintType)
enum class EModifierStacking : uint8
{
	StackAdditively,
	StackMultiplicatively,
	StrongestOnly,
	WeakestOnly,
	SourceUnique
};


USTRUCT(BlueprintType)
struct FStatModifier
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Stats"))
	FGameplayTag TargetStat;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStatModifierType ModType = EStatModifierType::Increased;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EModifierStacking StackingType = EModifierStacking::StackAdditively;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories = "Condition"))
	FGameplayTagQuery ConditionQuery;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag SourceType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SourceName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Damage,Attack,Spell,Element"))
	FGameplayTagContainer ModifierTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Stats"))
	FGameplayTag ConvertFromStat;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "ModType == EStatModifierType::Convert"))
	float ConversionEfficiency = 1.f;
	
	bool operator==(const FStatModifier& Other) const
	{
		return TargetStat == Other.TargetStat &&
			SourceName == Other.SourceName && 
			SourceType == Other.SourceType &&
			ModType == Other.ModType;
						
	}
};

USTRUCT(BlueprintType)
struct FActiveModifier
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FStatModifier Data;
	
	UPROPERTY(BlueprintReadOnly)
	FGuid SourceId;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsActive = false;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsInfinite = true;
	// Duration and remaining time should be int because they will depend on action points not the delta time 	
	UPROPERTY(BlueprintReadOnly)
	int32 Duration = 0;
	
	UPROPERTY(BlueprintReadOnly)
	int32 RemainingTime = 0;
	
	UPROPERTY(BlueprintReadOnly)
	int32 StackCount = 1;
	
	bool operator==(const FActiveModifier& Other) const
	{
		return SourceId == Other.SourceId && Data == Other.Data;
	}
	
};

USTRUCT()
struct FModifierBuckets
{
	GENERATED_BODY()
	
	float Base = 0.f;
	float FlatAdd = 0.f;
	
	float IncreasedTotal = 0.f;
	float ReducedTotal = 0.f;
	
	TArray<float> MoreMultipliers;
	TArray<float> LessMultipliers;
	
	TOptional<float> OverrideValue;
	
	TMap<FGameplayTag, float> ConvertFrom;
	
	void Reset()
	{
		Base = 0.f;
		FlatAdd = 0.f;
		IncreasedTotal = 0.f;
		ReducedTotal = 0.f;
		MoreMultipliers.Empty();
		LessMultipliers.Empty();
		OverrideValue.Reset();
		ConvertFrom.Empty();
	}
	
	float CalculateFinal() const
	{
		if (OverrideValue.IsSet())
		{
			return OverrideValue.GetValue();
		}
		
		float AdditiveFactor = FMath::Max(0.f, 1.f + (IncreasedTotal - ReducedTotal) / 100.f);
		
		float MultiplicativeFactor = 1.f;
		for (float More : MoreMultipliers)
		{
			MultiplicativeFactor *= (1.f + (More / 100.f));
		}
		
		for (float Less : LessMultipliers)
		{
			MultiplicativeFactor *= (1.f - (Less / 100.f));
		}
		
		return (Base + FlatAdd) * AdditiveFactor * MultiplicativeFactor;
		
	}
};

USTRUCT(BlueprintType)
struct FStatInstance
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	float CurrentValue = 0.f;
	
	UPROPERTY()
	float BaseValue = 0.f;
	
	UPROPERTY()
	float MinValue = 0.f;
	
	UPROPERTY()
	float MaxValue = 100.f;
	
	UPROPERTY()
	bool bIsResource = false;
	
	UPROPERTY()
	FGameplayTag LinkedBaseStat;
	
	UPROPERTY()
	FGameplayTag LinkedCurrentStat;
	
	UPROPERTY()
	FModifierBuckets Buckets;

	void Initialize(float InBaseValue, float InMin = 0.f, float InMax = 0.f, bool bInIsResource = false)
	{
		BaseValue = InBaseValue;
		MinValue = InMin;
		MaxValue = InMax;
		bIsResource = bInIsResource;
		Buckets.Base = InBaseValue;
	}
	
	void Recalculate()
	{
			CurrentValue = Buckets.CalculateFinal();
			CurrentValue = FMath::Clamp(CurrentValue, MinValue, MaxValue);
	}
};

USTRUCT(Blueprintable)
struct FStatScalingRule
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Stats"))
	FGameplayTag SourceStat;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Stats"))
	FGameplayTag TargetStat;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ScalePerUnit = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseValue = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStatModifierType ModifierType = EStatModifierType::FlatAdd;
	
	FStatModifier CreateModifier(float SourceValue) const
	{
		FStatModifier Mod;
		Mod.TargetStat = TargetStat;
		Mod.Value = BaseValue + (SourceValue * ScalePerUnit);
		Mod.ModType = ModifierType;
		Mod.SourceName = FName(*FString::Printf(TEXT("Scaling_%s_To_%s"), *SourceStat.ToString(), *TargetStat.ToString()));
		//Mod.SourceType = FGameplayTag::RequestGameplayTag(FName("")); Make a new tag for this.
		
		return Mod;
	}
	
	bool operator==(const FStatScalingRule& Other) const
	{
		return SourceStat == Other.SourceStat &&
			TargetStat == Other.TargetStat &&
			ModifierType == Other.ModifierType;
	}
};

USTRUCT()
struct FStatDependencyNode 
{
	GENERATED_BODY()
	
	UPROPERTY()
	FGameplayTag StatTag;
	
	UPROPERTY()
	TSet<FGameplayTag> Dependencies;
	
	UPROPERTY()
	TSet<FGameplayTag> Dependents;
	
	UPROPERTY()
	TArray<FStatScalingRule> ScalingRules;
	
};
