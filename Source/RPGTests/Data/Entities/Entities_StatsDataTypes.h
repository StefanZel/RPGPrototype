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


USTRUCT()
struct FStatLimits
{
	GENERATED_BODY()
	
	float MinValue = 0.f;
	float SoftCap = 0.f;
	float HardCap = 0.f;
	bool bCanExceedSoftCap = true;
};


USTRUCT(BlueprintType)
struct FStatModifier
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Stat"))
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Stat"))
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
	
	float SoftCap = 0.f;
	float HardCap = 0.f;
	float MinValue = 0.f;
	bool bCanExceedSoftCap = true;
	
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
	
	float CalculateFinal(const FStatLimits& Limits) const
	{
		if (OverrideValue.IsSet())
		{
			float Val = OverrideValue.GetValue();
			if (Limits.HardCap > 0.f) Val = FMath::Min(Val, Limits.HardCap);
			return FMath::Max(Val, Limits.MinValue);
		}
		
		float AdditiveFactor = FMath::Max(0.f, 1.f + (IncreasedTotal - ReducedTotal) / 100.f);
		
		// TODO: This is some nonsense for now, will come back to this later.
		float PreMultiply = Base + FlatAdd;
		if (Limits.SoftCap > 0.f && PreMultiply > Limits.SoftCap)
		{
			const float AboveCap = PreMultiply - Limits.SoftCap;
			PreMultiply = Limits.SoftCap + AboveCap * 0.33f;
		}
		
		float MultiplicativeFactor = 1.f;
		for (float More : MoreMultipliers)
		{
			MultiplicativeFactor *= (1.f + (More / 100.f));
		}
		
		for (float Less : LessMultipliers)
		{
			MultiplicativeFactor *= (1.f - (Less / 100.f));
		}
		float Result = PreMultiply * AdditiveFactor * MultiplicativeFactor;
		
		if (Limits.HardCap > 0.f)
		{
			Result = FMath::Min(Result, Limits.HardCap);
		}
		return FMath::Max(Result, Limits.MinValue);
		
	}
};

USTRUCT(BlueprintType)
struct FStatInstance
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	float CurrentValue = 0.f;
	
	UPROPERTY(BlueprintReadOnly)
	float BaseValue = 0.f;

	UPROPERTY()
	FStatLimits Limits;
	
	UPROPERTY()
	FModifierBuckets Buckets;

	void Initialize(float InBaseValue, const FStatLimits& InLimits = FStatLimits())
	{
		BaseValue = InBaseValue;
		Buckets.Base = InBaseValue;
		Limits = InLimits;
	}
	
	void Recalculate()
	{
			CurrentValue = Buckets.CalculateFinal(Limits);
	}
};

USTRUCT(BlueprintType)
struct FResourceInstance
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	FStatInstance Max;
	
	
	UPROPERTY(BlueprintReadOnly)
	float Current = 0.f;
	
	UPROPERTY()
	float MinValue = 0.f;
		
	float GetPercent() const
	{
		return Max.CurrentValue > 0.f ? FMath::Clamp(Current / Max.CurrentValue, 0.f, 1.f) : 0.f;
	}
	
	void FillToMax()
	{
		Current = Max.CurrentValue;
	}
	
	void OnMaxChanged(float OldMax, float NewMax)
	{
		if (OldMax > 0.f)
		{
			Current = FMath::Clamp(Current * (NewMax / OldMax), MinValue, NewMax);
		}
		else
		{
			Current = NewMax;
		}
	}
	
	float ApplyDelta(float Delta)
	{
		const float OldCurrent = Current;
		Current = FMath::Clamp(Current + Delta, MinValue, Max.CurrentValue);
		return Current - OldCurrent;
	}
	
	void Initialize(float InMaxValue, float InMinValue = 0.f)
	{
		MinValue = InMinValue;
		Max.Initialize(InMaxValue, FStatLimits());
		Current = InMaxValue;
	}
};

USTRUCT(Blueprintable)
struct FStatScalingRule
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Stat"))
	FGameplayTag SourceStat;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Stat"))
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
