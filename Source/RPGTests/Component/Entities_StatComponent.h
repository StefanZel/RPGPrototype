// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RPGTests/Data/Entities/Entities_DataAssetMain.h"
#include "Entities_StatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStatChanged, FGameplayTag, StatTag, float, OldValue, float, NewValue);
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
	
	float GetStatValue(FGameplayTag StatTag);
	float GetBaseStatValue(FGameplayTag StatTag) const;
	void SetBaseStatValue(FGameplayTag StatTag, float NewValue);
	FStatInstance GetStatInstance(FGameplayTag StatTag) const;
	TMap<FGameplayTag, FStatInstance> GetAllStats() const { return StatInstance; }
	
	float GetCurrentResource(FGameplayTag ResourceTag) const;
	void SetCurrentResource(FGameplayTag ResourceTag, float NewValue);
	float ModifyCurrentResource(FGameplayTag ResourceTag, float Delta);
	float GetMaxResource(FGameplayTag ResourceTag);
	bool HasEnoughResources(FGameplayTag ResourceTag, float Amount) const;
	
	FGuid AddModifier(const FStatModifier& Modifier, int32 Duration = 0, int32 Stacks = 1);
	bool RemoveModifier(FGuid ModifierId);
	void RemoveModifierBySource(const FName& SourceName);
	void RemoveModifierBySourceType(FGameplayTag SourceType);
	void ClearAllModifiers();
	TArray<FActiveModifier> GetActiveModifiers() const { return ActiveModifiers; }
	TArray<FActiveModifier> GetModifiersForStat(FGameplayTag StatTag) const;
	
	void AddStateTag(FGameplayTag StateTag);
	void RemoveStateTag(FGameplayTag StateTag);
	bool HasStateTag(FGameplayTag StateTag) const;
	FGameplayTagContainer GetStateTags() const { return EntityStateTags; }
	
	void RecalculateStatAndDependants(FGameplayTag StatTag);
	void RecalculateAllStats();
	
	void MarkStatDirty(FGameplayTag StatTag);
	void MarkStatAndDependentsDirty(FGameplayTag StatTag);
	
	
	void DebugDumpStats();
	
	UPROPERTY(BlueprintAssignable, Category="Stats|Events")
	FOnStatChanged OnStatChanged;
	
	UPROPERTY(BlueprintAssignable, Category="Stats|Events")
	FOnModifierApplied OnModifierApplied;
	
	UPROPERTY(BlueprintAssignable, Category="Stats|Events")
	FOnModifierApplied OnModifierRemoved;
	
protected:
	void InitializeBaseStats(UEntities_DataAssetMain* DataAsset);
	void RegisterScalingRules(UEntities_DataAssetMain* DataAsset);
	
	void BuildDependencyGraph();
	void BuildCalculationOrder();
	
	void ExecuteStatCalculation(FGameplayTag StatTag);
	//void RecalculateStatFrom(FGameplayTag StartStatTag);
	void GatherModifiersFromStat(FGameplayTag StatTag, FModifierBuckets& OutBuckets);
	void RecalculateCurrentStat(const FGameplayTag& StatTag, float NewValue, float OldMax);
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
	TArray<FActiveModifier> ActiveModifiers;
	
	UPROPERTY()
	FGameplayTagContainer EntityStateTags;
	
	UPROPERTY()
	TMap<FGuid, int32> ModifierIndexMap;
	
	UPROPERTY()
	TMap<FGameplayTag, FStatDependencyNode> DependencyGraph;
	
	UPROPERTY()
	TMap<FGameplayTag, float> BaseStatValue;
	
	UPROPERTY()
	TMap<FGameplayTag, float> CurrentResourceValues;
	
	UPROPERTY()
	TArray<FGameplayTag> CalculationOrder;
	
	UPROPERTY()
	TArray<FStatScalingRule> EntityScalingRules;
	
	UPROPERTY()
	TSet<FGameplayTag> DirtyStats;
	
	bool bIsRecalculating;
	bool bGraphUpToDate;
	

private:
	void InitializeStat(FGameplayTag StatTag, float BaseValue, float MinValue = 0.f, float MaxValue = 999999.f, bool bIsResource = false);
	void ValidateStatInstance();
	UEntities_DataAssetMain* GetEntityData() const;

};
