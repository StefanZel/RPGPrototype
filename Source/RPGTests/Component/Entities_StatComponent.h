// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RPGTests/Data/Entities/Entities_DataAssetMain.h"
#include "RPGTests/Data/Entities/StatConfigDataAsset.h"
#include "Entities_StatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStatChanged, FGameplayTag, StatTag, float, OldValue, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnResourceChanged, FGameplayTag, ResourceTag, float, Current, float, Max);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModifierApplied, const FActiveModifier&, Modifier);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModifierRemoved, const FActiveModifier&, Modifier);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RPGTESTS_API UEntities_StatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEntities_StatComponent(const FObjectInitializer& ObjectInitializer);
	void Initialize(const FPrimaryAssetId& NewEntityAssetId);
	
	UFUNCTION()
	static UEntities_StatComponent* FindEntityStatComponent(const AActor* Entity) { return (Entity ? Entity->FindComponentByClass<UEntities_StatComponent>() : nullptr); }
	
	void ResetStats();
	
	UFUNCTION(BlueprintCallable)
	float GetStatValue(FGameplayTag StatTag);
	float GetBaseStatValue(FGameplayTag StatTag);
	void SetBaseStatValue(FGameplayTag StatTag, float NewValue);
	
	UFUNCTION(BlueprintCallable)
	FStatInstance GetStatInstance(FGameplayTag StatTag) const;
	
	UFUNCTION(BlueprintCallable)
	TMap<FGameplayTag, FStatInstance> GetAllStats() const { return StatInstance; }
	
	UFUNCTION(BlueprintCallable)
	float GetResourceCurrent(FGameplayTag ResourceTag) const;
	
	void SetCurrentResource(FGameplayTag ResourceTag, float NewValue);
	float ModifyCurrentResource(FGameplayTag ResourceTag, float Delta);
	
	UFUNCTION(BlueprintCallable)
	float GetResourceMax(FGameplayTag ResourceTag);
	UFUNCTION(BlueprintCallable)
	bool HasEnoughResources(FGameplayTag ResourceTag, float Amount) const;
	
	void FillResource(FGameplayTag ResourceTag);
	void FillAllResources();
	
	UFUNCTION(BlueprintCallable)
	TMap<FGameplayTag, FResourceInstance> GetAllResources() const { return ResourceInstance; }
	
	FGuid AddModifier(const FStatModifier& Modifier, int32 Duration = 0, int32 Stacks = 1);
	bool RemoveModifier(FGuid ModifierId);
	void RemoveModifierBySource(const FName& SourceName);
	void RemoveModifierBySourceType(FGameplayTag SourceType);
	void ClearAllModifiers();
	
	UFUNCTION(BlueprintCallable)
	TArray<FActiveModifier> GetActiveModifiers() const;
	UFUNCTION(BlueprintCallable)
	TArray<FActiveModifier> GetModifiersForStat(FGameplayTag StatTag) const;
	
	void TickModifiers(int32 ActionPointsSpent);
	
	void AddStateTag(FGameplayTag StateTag);
	void RemoveStateTag(FGameplayTag StateTag);
	bool HasStateTag(FGameplayTag StateTag) const;
	FGameplayTagContainer GetStateTags() const { return EntityStateTags; }
	
	void RecalculateAllStats();
	
	void MarkStatDirty(FGameplayTag StatTag);
	void MarkStatAndDependentsDirty(FGameplayTag StatTag);
	
	
	void DebugDumpStats();
	
	UPROPERTY(BlueprintAssignable, Category="Stats|Events")
	FOnStatChanged OnStatChanged;
	
	UPROPERTY(BlueprintAssignable, Category="Stats|Events")
	FOnResourceChanged OnResourceChanged;
	
	UPROPERTY(BlueprintAssignable, Category="Stats|Events")
	FOnModifierApplied OnModifierApplied;
	
	UPROPERTY(BlueprintAssignable, Category="Stats|Events")
	FOnModifierRemoved OnModifierRemoved;
	
protected:
	void InitializeBaseStats(UEntities_DataAssetMain* DataAsset);
	void InitializeResources(UEntities_DataAssetMain* DataAsset);
	void RegisterScalingRules(UEntities_DataAssetMain* DataAsset);
	void ApplyConfigModifiers(UEntities_DataAssetMain* DataAsset);
	
	void BuildDependencyGraph();
	void BuildCalculationOrder();
	
	void ExecuteStatCalculation(FGameplayTag StatTag);
	void GatherModifiersFromStat(FGameplayTag StatTag, FModifierBuckets& OutBuckets);
	void OnResourceMaxChanged(const FGameplayTag& ResourceTag, float OldMax, float NewMax);
	bool EvaluateConditionQuery(const FGameplayTagQuery& Query) const;
	void ApplyModifierToBucket(FModifierBuckets& Buckets, EStatModifierType ModType, float Value) const;
	
	void ApplyModifierInternal(const FActiveModifier& Modifier);
	void RemoveModifierInternal(const FActiveModifier& Modifier);
	void OnModifierExpired(FGuid ModifierId);
	void RecalculateConditionalModifiers();
	
	void AddDependency(FGameplayTag Source, FGameplayTag Target);
	void AddScalingRule(const FStatScalingRule& Rule);
	void LinkScalingRule(const FStatScalingRule& Rule);
	void ClearAllScalingRules();
	
	void PropagateDirtyFlags(FGameplayTag StatTag);
	void RecalculateDirtyStats();
	
	
	UPROPERTY()
	FPrimaryAssetId EntityAssetId;
	
	UPROPERTY()
	TMap<FGameplayTag, FStatInstance> StatInstance;
	
	UPROPERTY()
	TMap<FGameplayTag, FResourceInstance> ResourceInstance;
	
	UPROPERTY()
	TMap<FGuid, FActiveModifier> ActiveModifiers;

	UPROPERTY()
	FGameplayTagContainer EntityStateTags;
	
	UPROPERTY()
	TMap<FGameplayTag, FStatDependencyNode> DependencyGraph;
	
	UPROPERTY()
	TArray<FGameplayTag> CalculationOrder;
	
	UPROPERTY()
	TArray<FStatScalingRule> EntityScalingRules;
	
	UPROPERTY()
	TSet<FGameplayTag> DirtyStats;
	
	bool bIsRecalculating;
	bool bGraphUpToDate;
	

private:
	void InitializeStat(FGameplayTag StatTag, float BaseValue);
	void InitializeResource(FGameplayTag StatTag, float MaxValue = 999999.f);
	void ValidateStatInstance();
	UEntities_DataAssetMain* GetEntityData() const;
	const UStatConfigDataAsset* GetStatConfig() const;

};
