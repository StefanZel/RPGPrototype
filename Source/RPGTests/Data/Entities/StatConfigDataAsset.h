// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Entities_StatsDataTypes.h"
#include "Engine/DataAsset.h"
#include "StatConfigDataAsset.generated.h"


USTRUCT(BlueprintType)
struct FStatConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Stat"))
	FGameplayTag StatTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DefaultBaseValue = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HardCap = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SoftCap = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinValue = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCanExceedSoftCap = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bRefillOnCombatStart = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FStatModifier> GlobalModifiers;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FStatScalingRule> GlobalScalingRules;
};
// This will change later, will probably need more params for resources
USTRUCT(BlueprintType)
struct FResourceConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Stat.Resource"))
	FGameplayTag ResourceTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DefaultMaxValue = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HardCap = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SoftCap = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinValue = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCanExceedSoftCap = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bRefillOnCombatStart = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FStatModifier> GlobalModifiers;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FStatScalingRule> GlobalScalingRules;
};
/**
 * 
 */
UCLASS()
class RPGTESTS_API UStatConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (GameplayTagFilter = "Stat"))
	TMap<FGameplayTag, FStatConfig> Configs;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (GameplayTagFilter = "Stat.Resource"))
	TMap<FGameplayTag, FResourceConfig> ResourceConfigs;
	
	const FStatConfig* GetConfig(const FGameplayTag& StatTag) const
	{
		return Configs.Find(StatTag);
	}
	
	bool HasConfig(const FGameplayTag& StatTag) const
	{
		return Configs.Contains(StatTag);
	}
	
	const FResourceConfig* GetResourceConfig(const FGameplayTag& ResourceTag) const
	{
		return ResourceConfigs.Find(ResourceTag);
	}
	
	bool HasResourceConfig(const FGameplayTag& ResourceTag) const
	{
		return ResourceConfigs.Contains(ResourceTag);
	}
	
	float ResolveBaseValue(const FGameplayTag& StatTag, const TMap<FGameplayTag, float>& EntityOverrides) const
	{
		if (const float* Override = EntityOverrides.Find(StatTag))
		{
			return *Override;
		}
		if (const FStatConfig* Config = GetConfig(StatTag))
		{
			return Config->DefaultBaseValue;
		}
		return 0.f;
	}
	
	float ResolveResourceMaxValue(const FGameplayTag& ResourceTag, const TMap<FGameplayTag, float>& EntityOverrides) const
	{
		if (const float* Override = EntityOverrides.Find(ResourceTag))
		{
			return *Override;
		}
		if (const FResourceConfig* Config = GetResourceConfig(ResourceTag))
		{
			return Config->DefaultMaxValue;
		}
		return 0.f;	
	}
	
	float ResolveHardCap(const FGameplayTag& StatTag, float FallbackMax) const
	{
		if (const FStatConfig* Config = GetConfig(StatTag))
		{
			return Config->HardCap > 0.f ? Config->HardCap : FallbackMax;
		}
		return FallbackMax;
	}
	
	// TODO: Soft cap will work differently, but this will work for now.
	float ResolveSoftCap(const FGameplayTag& StatTag) const
	{
		if (const FStatConfig* Config = GetConfig(StatTag))
		{
			return Config->SoftCap;
		}
		return 0.f;
	}
};
