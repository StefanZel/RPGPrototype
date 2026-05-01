
#include "Entities_StatComponent.h"

#include "Engine/AssetManager.h"
#include "RPGTests/Data/Entities/Entities_Tags.h"
#include "RPGTests/Data/Entities/StatConfigDataAsset.h"
#include "RPGTests/SubSystems/StatConfigSubsystem.h"


UEntities_StatComponent::UEntities_StatComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	bIsRecalculating = false;
	bGraphUpToDate = false;

}

void UEntities_StatComponent::Initialize(const FPrimaryAssetId& NewEntityAssetId)
{
	EntityAssetId = NewEntityAssetId;
	
	UEntities_DataAssetMain* DataAsset = GetEntityData();
	if (!DataAsset) return;
	
	ResetStats();
	
	InitializeBaseStats(DataAsset);
	InitializeResources(DataAsset);
	
	for (const FStatScalingRule& Rule : DataAsset->EntityScalingRule)
	{
		if (!StatInstance.Contains(Rule.TargetStat) && !ResourceInstance.Contains(Rule.TargetStat))
		{
			InitializeStat(Rule.TargetStat, 0.f);
		}
	}
	
	EntityScalingRules = DataAsset->EntityScalingRule;
	BuildDependencyGraph();
	BuildCalculationOrder();
	
	// TODO: Apply config modifier
	ApplyConfigModifiers(DataAsset);
	for (const FStatModifier& Modifier : DataAsset->StartingModifier)
	{
		AddModifier(Modifier);
	}
	
	RecalculateConditionalModifiers();
	
	RecalculateAllStats();
	
	// TODO: This will probably make problems later
	FillAllResources();
	for (const auto& [Tag, Resource] : ResourceInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("POST-INIT Resource %s: %.1f / %.1f"),
			*Tag.ToString(), Resource.Current, Resource.Max.CurrentValue);
	}
}

void UEntities_StatComponent::ResetStats()
{
	StatInstance.Empty();
	ResourceInstance.Empty();
	ActiveModifiers.Empty();
	CalculationOrder.Empty();
	EntityScalingRules.Empty();
	EntityStateTags.Reset();
	DirtyStats.Empty();
	
	bGraphUpToDate = false;
	bIsRecalculating = false;
}

float UEntities_StatComponent::GetStatValue(FGameplayTag StatTag)
{
	if (!StatTag.IsValid()) return 0.f;
	
	if (DirtyStats.Contains(StatTag))
	{
		RecalculateDirtyStats();
	}
	
	if (const FStatInstance* Instance = StatInstance.Find(StatTag))
	{
		return Instance->CurrentValue;
	}
	
	return 0.f;
}

float UEntities_StatComponent::GetBaseStatValue(FGameplayTag StatTag)
{
	if (!StatTag.IsValid()) return 0.f;
	
	if (const FStatInstance* Instance = StatInstance.Find(StatTag))
	{
		return Instance->BaseValue;
	}
	
	if (const FResourceInstance* Resource = ResourceInstance.Find(StatTag))
	{
		return Resource->Max.BaseValue;
	}

	return 0.f;
}

void UEntities_StatComponent::SetBaseStatValue(FGameplayTag StatTag, float NewValue)
{
	if (!StatTag.IsValid()) return;
	
	
	if (FResourceInstance* Resource = ResourceInstance.Find(StatTag))
	{
		if (!FMath::IsNearlyEqual(Resource->Max.BaseValue,NewValue))
		{
			Resource->Max.BaseValue = NewValue;
			MarkStatAndDependentsDirty(StatTag);
			RecalculateDirtyStats();
		}
		return;
	}
	
	
	FStatInstance* Instance = StatInstance.Find(StatTag);
	if (!Instance)
	{
		InitializeStat(StatTag, NewValue);
		Instance = StatInstance.Find(StatTag);
	}
	
	if (Instance)
	{
		float OldValue = Instance->BaseValue;
		if (!FMath::IsNearlyEqual(OldValue, NewValue))
		{
			Instance->BaseValue = NewValue;
		
			MarkStatAndDependentsDirty(StatTag);
			RecalculateDirtyStats();
		
			OnStatChanged.Broadcast(StatTag, OldValue, Instance->CurrentValue);
		}
	}
	
}

FStatInstance UEntities_StatComponent::GetStatInstance(FGameplayTag StatTag) const
{
	if (const FStatInstance* Instance = StatInstance.Find(StatTag))
	{
		return *Instance;
	}
	return FStatInstance();
}

float UEntities_StatComponent::GetResourceCurrent(FGameplayTag ResourceTag) const
{
	if (const FResourceInstance* Resource = ResourceInstance.Find(ResourceTag))
	{
		return Resource->Current;
	}
	
	return 0.f;
}

void UEntities_StatComponent::SetCurrentResource(FGameplayTag ResourceTag, float NewValue)
{
	FResourceInstance* Resource = ResourceInstance.Find(ResourceTag);
	if (!Resource) return;
	
	float OldValue = Resource->Current;
	
	Resource->Current = FMath::Clamp(NewValue, Resource->MinValue, Resource->Max.CurrentValue);
	
	if (!FMath::IsNearlyEqual(OldValue,Resource->Current))
	{
		OnResourceChanged.Broadcast(ResourceTag, Resource->Current, Resource->Max.CurrentValue);
	}
}

float UEntities_StatComponent::ModifyCurrentResource(FGameplayTag ResourceTag, float Delta)
{
	FResourceInstance* Resource = ResourceInstance.Find(ResourceTag);
	if (!Resource) return 0.f;
	
	const float OldCurrent = Resource->Current;
	Resource->ApplyDelta(Delta);
	
	if (!FMath::IsNearlyEqual(OldCurrent, Resource->Current))
	{
		OnResourceChanged.Broadcast(ResourceTag, Resource->Current, Resource->Max.CurrentValue);
		
		// Death check meh
		if (ResourceTag == EntityTags::Stat::Resources::Health && Resource->Current <= 0.f)
		{
			RemoveStateTag(EntityTags::State::Alive);
			AddStateTag(EntityTags::State::Dead);
		}
	}
	
	return Resource->Current;
}

float UEntities_StatComponent::GetResourceMax(FGameplayTag ResourceTag)
{
	if (DirtyStats.Contains(ResourceTag))
	{
		RecalculateDirtyStats();
	}
	
	if (const FResourceInstance* Resource = ResourceInstance.Find(ResourceTag))
	{
		return Resource->Max.CurrentValue;
	}
	return 0.f;
}

bool UEntities_StatComponent::HasEnoughResources(FGameplayTag ResourceTag, float Amount) const
{
	return GetResourceCurrent(ResourceTag) >= Amount;
}

void UEntities_StatComponent::FillResource(FGameplayTag ResourceTag)
{
	FResourceInstance* Resource = ResourceInstance.Find(ResourceTag);
	if (!Resource) return;
	
	Resource->FillToMax();
	OnResourceChanged.Broadcast(ResourceTag, Resource->Current, Resource->Max.CurrentValue);
}

void UEntities_StatComponent::FillAllResources()
{
	for (auto& [Tag, Resource] : ResourceInstance)
	{
		Resource.FillToMax();
		OnResourceChanged.Broadcast(Tag, Resource.Current, Resource.Max.CurrentValue);
	}
}

FGuid UEntities_StatComponent::AddModifier(const FStatModifier& Modifier, int32 Duration, int32 Stacks)
{
	FActiveModifier NewModifier;
	NewModifier.Data = Modifier;
	NewModifier.SourceId = FGuid::NewGuid();
	NewModifier.bIsActive = EvaluateConditionQuery(Modifier.ConditionQuery);
	NewModifier.bIsInfinite = Duration <= 0;
	NewModifier.Duration = Duration;
	NewModifier.RemainingTime = Duration;
	NewModifier.StackCount = FMath::Max(1, Stacks);
	
	ActiveModifiers.Add(NewModifier.SourceId, NewModifier);
	
	if (NewModifier.bIsActive)
	{
		ApplyModifierInternal(NewModifier);
	}
	
	if (!NewModifier.bIsInfinite)
	{
		// TODO: Handle duration here
	}
	OnModifierApplied.Broadcast(NewModifier);
	
	return NewModifier.SourceId;
}

bool UEntities_StatComponent::RemoveModifier(FGuid ModifierId)
{
	if (FActiveModifier* ModifierPtr = ActiveModifiers.Find(ModifierId))
	{
		FActiveModifier Modifier = *ModifierPtr;
		ActiveModifiers.Remove(ModifierId);
		
		RemoveModifierInternal(Modifier);
		// TODO: Handle duration timer if needed
		OnModifierRemoved.Broadcast(Modifier);
		return true;
	}
	return false;
}

void UEntities_StatComponent::RemoveModifierBySource(const FName& SourceName)
{
	TArray<FGuid> ModifiersToRemove;
	
	for (auto& Pair: ActiveModifiers)
	{
		if (Pair.Value.Data.SourceName == SourceName)
		{
			ModifiersToRemove.Add(Pair.Key);
		}
	}

	for (const FGuid& Id : ModifiersToRemove)
	{
		RemoveModifier(Id);
	}
}

void UEntities_StatComponent::RemoveModifierBySourceType(FGameplayTag SourceType)
{
	TArray<FGuid> ModifiersToRemove;
	
	for (auto& Pair: ActiveModifiers)
	{
		if (Pair.Value.Data.SourceType == SourceType)
		{
			ModifiersToRemove.Add(Pair.Key);
		}
	}
	for (const FGuid& Id : ModifiersToRemove)
	{
		RemoveModifier(Id);
	}
}

void UEntities_StatComponent::ClearAllModifiers()
{
	TArray<FGuid> AllModifierIds;
	for (const auto& Pair : ActiveModifiers)
	{
		AllModifierIds.Add(Pair.Key);
	}
	
	for (const FGuid& Id : AllModifierIds)
	{
		RemoveModifier(Id);
	}
	
}

TArray<FActiveModifier> UEntities_StatComponent::GetActiveModifiers() const
{
	TArray<FActiveModifier> OutModifiers;
	ActiveModifiers.GenerateValueArray(OutModifiers);
	return OutModifiers;
}

TArray<FActiveModifier> UEntities_StatComponent::GetModifiersForStat(FGameplayTag StatTag) const
{
	TArray<FActiveModifier> Result;
	for (auto& Pair: ActiveModifiers)
	{
		if (Pair.Value.Data.TargetStat == StatTag)
		{
			Result.Add(Pair.Value);
		}
	}
	return Result;
}

void UEntities_StatComponent::TickModifiers(int32 ActionPointsSpent)
{
	TArray<FGuid> Expired;
	
	for (auto& [Id, Mod] : ActiveModifiers)
	{
		if (Mod.bIsInfinite) continue;
		
		Mod.RemainingTime -= ActionPointsSpent;
		if (Mod.RemainingTime <= 0)
			Expired.Add(Id);
	}
	
	for (const FGuid& Id : Expired)
	{
		RemoveModifier(Id);
	}
}

void UEntities_StatComponent::AddStateTag(FGameplayTag StateTag)
{
	if (!EntityStateTags.HasTag(StateTag))
	{
		EntityStateTags.AddTag(StateTag);
		RecalculateConditionalModifiers();
	}
}

void UEntities_StatComponent::RemoveStateTag(FGameplayTag StateTag)
{
	if (EntityStateTags.HasTag(StateTag))
	{
		EntityStateTags.RemoveTag(StateTag);
		RecalculateConditionalModifiers();
	}	
}

bool UEntities_StatComponent::HasStateTag(FGameplayTag StateTag) const
{
	return EntityStateTags.HasTag(StateTag);
}

void UEntities_StatComponent::RecalculateAllStats()
{
	if (!bGraphUpToDate)
	{
		BuildCalculationOrder();
	}
	
	bIsRecalculating = true;
	
	for (const FGameplayTag& StatTag : CalculationOrder)
	{
		ExecuteStatCalculation(StatTag);
	}
	
	bIsRecalculating = false;
}

void UEntities_StatComponent::MarkStatDirty(FGameplayTag StatTag)
{
	if (!StatTag.IsValid()) return;
	DirtyStats.Add(StatTag);
}

void UEntities_StatComponent::MarkStatAndDependentsDirty(FGameplayTag StatTag)
{
	if (!StatTag.IsValid()) return;
	DirtyStats.Add(StatTag);
	
	PropagateDirtyFlags(StatTag);
}

void UEntities_StatComponent::DebugDumpStats()
{
	AActor* Owner = GetOwner();
    FString OwnerName = Owner ? Owner->GetName() : TEXT("Unknown");

    UE_LOG(LogTemp, Warning, TEXT("========== STAT DEBUG: [%s] =========="), *OwnerName);

    UE_LOG(LogTemp, Warning, TEXT("--- Calculated Stats ---"));
    for (const FGameplayTag& Tag : CalculationOrder)
    {
        
    	if (ResourceInstance.Contains(Tag)) continue;
        float FinalValue = GetStatValue(Tag);
        float BaseVal = GetBaseStatValue(Tag);
        FGameplayTag Category = EntityTags::GetStatCategory(Tag);
        
        
        UE_LOG(LogTemp, Log, TEXT("[%s] %s: %.2f (Base: %.2f) %s"), 
            *Category.GetTagName().ToString().Replace(TEXT("Stat."), TEXT("")), 
            *Tag.GetTagName().ToString(), 
            FinalValue, 
            BaseVal,
            !FMath::IsNearlyEqual(FinalValue, BaseVal) ? TEXT("*MODIFIED*") : TEXT(""));
    }
    
    TSet<FGameplayTag> ResourceBaseTags;
 	UE_LOG(LogTemp, Warning, TEXT("--- Resources ---"));
	for (const auto& [Tag, Resource] : ResourceInstance)
	{
		UE_LOG(LogTemp, Log, TEXT("%s: %.1f / %.1f (%.0f%%)"),
			*Tag.ToString(),
			Resource.Current,
			Resource.Max.CurrentValue,
			Resource.GetPercent() * 100.f);
	}
    

    UE_LOG(LogTemp, Warning, TEXT("--- Active Modifiers: %d ---"), ActiveModifiers.Num());
    for (const auto& Pair : ActiveModifiers)
    {
    	FActiveModifier Mod = Pair.Value;
        bool bShouldBeActive = EvaluateConditionQuery(Mod.Data.ConditionQuery);
        
        UE_LOG(LogTemp, Log, TEXT(" - [%s] %s: %s %+.2f (Stacks: %d) [Active: %s, ShouldBe: %s]"), 
            *Mod.SourceId.ToString(),
            *Mod.Data.TargetStat.ToString(),
            *StaticEnum<EStatModifierType>()->GetNameStringByValue((int64)Mod.Data.ModType),
            Mod.Data.Value,
            Mod.StackCount,
            Mod.bIsActive ? TEXT("YES") : TEXT("NO"),
            bShouldBeActive ? TEXT("YES") : TEXT("NO"));
    }
    
    UE_LOG(LogTemp, Warning, TEXT("--- Scaling Rules: %d ---"), EntityScalingRules.Num());
    for (const FStatScalingRule& Rule : EntityScalingRules)
    {
        float SourceValue = GetStatValue(Rule.SourceStat);
        float Bonus = Rule.BaseValue + (SourceValue * Rule.ScalePerUnit);
        
        UE_LOG(LogTemp, Log, TEXT(" - %s -> %s: %.2f + (%.2f * %.2f) = %.2f"), 
            *Rule.SourceStat.ToString(),
            *Rule.TargetStat.ToString(),
            Rule.BaseValue,
            SourceValue,
            Rule.ScalePerUnit,
            Bonus);
    }
    
    UE_LOG(LogTemp, Warning, TEXT("=========================================="));
}

void UEntities_StatComponent::InitializeBaseStats(UEntities_DataAssetMain* DataAsset)
{
	if (!DataAsset) return;
	
	const UStatConfigDataAsset* Config = GetStatConfig();
	
	if (Config)
	{
		for (const auto& [Tag, StatConf] : Config->Configs)
		{
			const float BaseValue = Config->ResolveBaseValue(Tag, DataAsset->BaseStats);
			InitializeStat(Tag, BaseValue);
		}
	}
	
	for (const auto& StatPair : DataAsset->BaseStats)
	{
		FGameplayTag StatTag = StatPair.Key;
		if (EntityTags::IsResourceTag(StatTag)) continue;
		
		float BaseValue =  StatPair.Value;
	
		InitializeStat(StatTag, BaseValue);
	}
}

void UEntities_StatComponent::InitializeResources(UEntities_DataAssetMain* DataAsset)
{
	if (!DataAsset) return;
	
	const UStatConfigDataAsset* Config = GetStatConfig();
	
	if (Config)
	{
		for (const auto& [Tag, ResConf] : Config->ResourceConfigs)
		{
			const float MaxValue = Config->ResolveResourceMaxValue(Tag, DataAsset->BaseStats);
			InitializeResource(Tag, MaxValue);
		}
	}
	
	for (const auto& [Tag, BaseValue] : DataAsset->BaseStats)
	{
		if (!EntityTags::IsResourceTag(Tag)) continue;
		if (ResourceInstance.Contains(Tag)) continue;
		
		FResourceInstance& Resource = ResourceInstance.FindOrAdd(Tag);
		Resource.Initialize(BaseValue, 0.f);
	}
}

void UEntities_StatComponent::RegisterScalingRules(UEntities_DataAssetMain* DataAsset)
{
	if (!DataAsset) return;
	
	EntityScalingRules = DataAsset->EntityScalingRule;
	
	for (auto& Pair : DependencyGraph)
	{
		Pair.Value.ScalingRules.Empty();
	}
	
	for (const FStatScalingRule& Rule: EntityScalingRules)
	{
		LinkScalingRule(Rule);
	}
}

void UEntities_StatComponent::ApplyConfigModifiers(UEntities_DataAssetMain* DataAsset)
{
	const UStatConfigDataAsset* Config = GetStatConfig();
	if (Config)
	{
		for (const auto& [Tag, ResConf] : Config->ResourceConfigs)
		{
			for (const FStatModifier& Mod : ResConf.GlobalModifiers)
			{
				AddModifier(Mod);
			}
		}
		for (const auto& [Tag, StatConf] : Config->Configs)
		{
			for (const FStatModifier& Mod : StatConf.GlobalModifiers)
			{
				AddModifier(Mod);
			}
		}	
	}
}

void UEntities_StatComponent::BuildDependencyGraph()
{
	DependencyGraph.Empty();
	
	for (const FStatScalingRule& Rule : EntityScalingRules)
	{
		LinkScalingRule(Rule);
	}
}

void UEntities_StatComponent::BuildCalculationOrder()
{
	CalculationOrder.Empty();
	
	TMap<FGameplayTag, int32> InDegree;
	TSet<FGameplayTag> AllInvolvedTags;
	
	for (const auto& Pair : StatInstance)
	{
		AllInvolvedTags.Add(Pair.Key);
	}
	
	for (const auto& Pair : ResourceInstance)
	{
		AllInvolvedTags.Add(Pair.Key);
	}
	
	for (const auto& Pair : DependencyGraph)
	{
		AllInvolvedTags.Add(Pair.Key);
		for (const FGameplayTag& Dependent : Pair.Value.Dependents)
		{
			AllInvolvedTags.Add(Dependent);
		}
	}
	
	for (const FGameplayTag& Tag : AllInvolvedTags)
	{
		InDegree.Add(Tag, 0);
	}
	
	for (const auto& Pair : DependencyGraph)
	{
		for (const FGameplayTag& Dependent : Pair.Value.Dependents)
		{
			int32& Degree = InDegree[Dependent];
			Degree++;
		}
	}
	
	TArray<FGameplayTag> Queue;
	for (const auto& Pair : InDegree)
	{
		if (Pair.Value == 0)
		{
			Queue.Add(Pair.Key);
		}
	}
	
	int32 Head = 0;
	while (Head < Queue.Num())
	{
		FGameplayTag Current = Queue[Head++];
		
		CalculationOrder.Add(Current);
		if (const FStatDependencyNode* Node = DependencyGraph.Find(Current))
		{
			for (const FGameplayTag& Dependent : Node->Dependents)
			{
				int32& Degree = InDegree[Dependent];
				Degree--;
				
				if (Degree == 0)
				{
					Queue.Add(Dependent);
				}
			}
		}
	}
	
	if (CalculationOrder.Num() < AllInvolvedTags.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Circular stat dependency detected! Forcing remaining stats."));
		
		for (const FGameplayTag& Tag : AllInvolvedTags)
		{
			if (!CalculationOrder.Contains(Tag))
			{
				CalculationOrder.Add(Tag);
			}
		}
	}
	
	bGraphUpToDate = true;
}

void UEntities_StatComponent::ExecuteStatCalculation(FGameplayTag StatTag)
{
	if (!StatTag.IsValid()) return;
	
	DirtyStats.Remove(StatTag);
	
	FStatInstance* StatInst = nullptr;;
	FResourceInstance* ResourceInst = ResourceInstance.Find(StatTag);
	if (ResourceInst)
	{
		StatInst = &ResourceInst->Max;
	}
	else
	{
		StatInst = StatInstance.Find(StatTag);
		if (!StatInst)
		{
			InitializeStat(StatTag, 0.f);
			StatInst = StatInstance.Find(StatTag);
			if (!StatInst) return;
		}
	}
	
	FModifierBuckets Buckets;
	Buckets.Base = GetBaseStatValue(StatTag);
	
	if (const FStatDependencyNode* Node = DependencyGraph.Find(StatTag))
	{
		for (const FStatScalingRule& Rule : Node->ScalingRules)
		{
			float SourceValue = 0.f;
			if (StatInstance.Contains(Rule.SourceStat))
			{
				SourceValue = GetStatValue(Rule.SourceStat);
			}
			else if (ResourceInstance.Contains(Rule.SourceStat))
			{
				SourceValue = GetResourceMax(Rule.SourceStat);
			}
			float Bonus = Rule.BaseValue + (SourceValue * Rule.ScalePerUnit);
			
			ApplyModifierToBucket(Buckets, Rule.ModifierType, Bonus);
		}
	}

	
	GatherModifiersFromStat(StatTag, Buckets);
	
	const float OldValue = StatInst->CurrentValue;
	
	StatInst->Buckets = Buckets;
	StatInst->Recalculate();
	const float NewValue = StatInst->CurrentValue;

	
	if (!FMath::IsNearlyEqual(OldValue, NewValue))
	{
		if (ResourceInst)
		{
			OnResourceMaxChanged(StatTag, OldValue, NewValue);
		}
		else
		{
			OnStatChanged.Broadcast(StatTag, OldValue, NewValue);
		}
	}
	
}

void UEntities_StatComponent::GatherModifiersFromStat(FGameplayTag StatTag, FModifierBuckets& OutBuckets)
{

	TMap<EStatModifierType, TArray<FStatModifier>> ModifiersByType;
	TMap<FGameplayTag, float> ConvertedValues;
	
	for (const auto& Pair : ActiveModifiers)
	{
		const FActiveModifier& Modifier = Pair.Value;
		if (!Modifier.bIsActive || Modifier.Data.TargetStat != StatTag) continue;
		
		if (Modifier.Data.ModType == EStatModifierType::Convert)
		{
			float SourceValue = GetStatValue(Modifier.Data.ConvertFromStat);
			float ConvertedAmount = SourceValue * Modifier.Data.ConversionEfficiency * Modifier.Data.Value;
			ConvertedValues.Add(Modifier.Data.ConvertFromStat, ConvertedValues.FindRef(Modifier.Data.ConvertFromStat) + ConvertedAmount);
		}
		else
		{
			FStatModifier StackedMod = Modifier.Data;
			StackedMod.Value *= Modifier.StackCount;
			ModifiersByType.FindOrAdd(Modifier.Data.ModType).Add(StackedMod);
		}
	}
	
	for (const auto& Pair : ConvertedValues)
	{
		OutBuckets.Base += Pair.Value;
		OutBuckets.ConvertFrom.Add(Pair.Key, Pair.Value);
	}
	
	//TODO: Should probably group by type after stacking here
	
	for (const auto& Pair : ModifiersByType)
	{
		EStatModifierType ModType = Pair.Key;
		const TArray<FStatModifier>& Modifiers = Pair.Value;

		switch (ModType) {
		case EStatModifierType::FlatAdd:
			for (const FStatModifier& Mod : Modifiers)
			{
				OutBuckets.FlatAdd += Mod.Value;
			}
			break;
		case EStatModifierType::Increased:
			for (const FStatModifier& Mod : Modifiers)
			{
				OutBuckets.IncreasedTotal += Mod.Value;
			}
			break;
		case EStatModifierType::Reduced:
			for (const FStatModifier& Mod : Modifiers)
			{
				OutBuckets.ReducedTotal += Mod.Value;
			}
			break;
		case EStatModifierType::More:
			for (const FStatModifier& Mod : Modifiers)
			{
				OutBuckets.MoreMultipliers.Add(Mod.Value);
			}
			break;
		case EStatModifierType::Less:
			for (const FStatModifier& Mod : Modifiers)
			{
				OutBuckets.LessMultipliers.Add(Mod.Value);
			}
			break;
		case EStatModifierType::Override:
			if (Modifiers.Num() > 0 )
			{
				float HighestOverride = Modifiers[0].Value;
				for (int32 i = 1; i < Modifiers.Num(); i++)
				{
					HighestOverride = FMath::Max(HighestOverride, Modifiers[i].Value);
				}
				OutBuckets.OverrideValue = HighestOverride;
			}
			break;
		}
	}
	
}

void UEntities_StatComponent::OnResourceMaxChanged(const FGameplayTag& ResourceTag, float OldMax, float NewMax)
{
	FResourceInstance* Resource = ResourceInstance.Find(ResourceTag);
	if (!Resource) return;
	
	Resource->OnMaxChanged(OldMax, NewMax);
	
	OnResourceChanged.Broadcast(ResourceTag, Resource->Current, NewMax);
}

bool UEntities_StatComponent::EvaluateConditionQuery(const FGameplayTagQuery& Query) const
{
	if (Query.IsEmpty())
	{
		return true;
	}
	return Query.Matches(EntityStateTags);
}

void UEntities_StatComponent::ApplyModifierToBucket(FModifierBuckets& Buckets, EStatModifierType ModType,
	float Value) const
{
	switch (ModType)
	{
	case EStatModifierType::FlatAdd:
		Buckets.FlatAdd += Value;
		break;
	case EStatModifierType::Increased:
		Buckets.IncreasedTotal += Value;
		break;
	case EStatModifierType::Reduced:
		Buckets.ReducedTotal += Value;
		break;
	case EStatModifierType::More:
		Buckets.MoreMultipliers.Add(Value);
		break;
	case EStatModifierType::Less:
		Buckets.LessMultipliers.Add(Value);
		break;
	case EStatModifierType::BaseValue:
		Buckets.Base = Value;
		break;
	case EStatModifierType::Override:
		Buckets.OverrideValue = Value;
		break;
    }	
}

void UEntities_StatComponent::ApplyModifierInternal(const FActiveModifier& Modifier)
{
	MarkStatAndDependentsDirty(Modifier.Data.TargetStat);
	//RecalculateDirtyStats();
	//RecalculateStatFrom(Modifier.Data.TargetStat);
}

void UEntities_StatComponent::RemoveModifierInternal(const FActiveModifier& Modifier)
{
	MarkStatAndDependentsDirty(Modifier.Data.TargetStat);
	//RecalculateDirtyStats();
	//RecalculateStatFrom(Modifier.Data.TargetStat);
}

void UEntities_StatComponent::OnModifierExpired(FGuid ModifierId)
{
	RemoveModifier(ModifierId);
}

void UEntities_StatComponent::RecalculateConditionalModifiers()
{
	bool bAnyChanged = false;
	
	for (auto& Pair : ActiveModifiers)
	{
		FActiveModifier& Modifier = Pair.Value;
		bool bShouldBeActive = EvaluateConditionQuery(Modifier.Data.ConditionQuery);
		
		if (Modifier.bIsActive != bShouldBeActive)
		{
			if (bShouldBeActive)
			{
				ApplyModifierInternal(Modifier);
			}
			else
			{
				RemoveModifierInternal(Modifier);
			}
			
			Modifier.bIsActive = bShouldBeActive;
			bAnyChanged = true;
		}
	}
	
	if (bAnyChanged)
	{
		RecalculateDirtyStats();
	}
	
}

void UEntities_StatComponent::AddDependency(FGameplayTag Source, FGameplayTag Target)
{
	if (!Source.IsValid() || !Target.IsValid()) return;
	FStatDependencyNode& SourceNode = DependencyGraph.FindOrAdd(Source);
	SourceNode.StatTag = Source;
	SourceNode.Dependents.Add(Target);
	
	FStatDependencyNode& TargetNode = DependencyGraph.FindOrAdd(Target);
	TargetNode.StatTag = Target;
	TargetNode.Dependencies.Add(Source);
	
	bGraphUpToDate = false;
}

void UEntities_StatComponent::AddScalingRule(const FStatScalingRule& Rule)
{
	if (!Rule.SourceStat.IsValid() || !Rule.TargetStat.IsValid()) return;
	
	EntityScalingRules.AddUnique(Rule);
	LinkScalingRule(Rule);
	
	bGraphUpToDate = false;
	
	if (!bIsRecalculating)
	{
		MarkStatAndDependentsDirty(Rule.TargetStat);
		RecalculateDirtyStats();
		//RecalculateStatFrom(Rule.TargetStat);
	}
}

void UEntities_StatComponent::LinkScalingRule(const FStatScalingRule& Rule)
{
	AddDependency(Rule.SourceStat, Rule.TargetStat);
	
	FStatDependencyNode& TargetNode = DependencyGraph.FindOrAdd(Rule.TargetStat);
	TargetNode.StatTag = Rule.TargetStat;
	TargetNode.ScalingRules.Add(Rule);
}

void UEntities_StatComponent::ClearAllScalingRules()
{
	EntityScalingRules.Empty();
	
	for (auto& Pair : DependencyGraph)
	{
		Pair.Value.ScalingRules.Empty();
	}
}

void UEntities_StatComponent::PropagateDirtyFlags(FGameplayTag StatTag)
{
	TArray<FGameplayTag> ToProcess;
	ToProcess.Add(StatTag);
	
	int32 Head = 0;
	while (Head < ToProcess.Num())
	{
		FGameplayTag Current = ToProcess[Head++];
		
		DirtyStats.Add(Current);
		
		if (const FStatDependencyNode* Node = DependencyGraph.Find(Current))
		{
			for (const FGameplayTag& Dependent : Node->Dependents)
			{
				if (!DirtyStats.Contains(Dependent))
				{
					ToProcess.Add(Dependent);
				}
			}
		}
	}
}

void UEntities_StatComponent::RecalculateDirtyStats()
{
	if (DirtyStats.Num() == 0 ) return;
	if (bIsRecalculating) return;
	bIsRecalculating = true;
	
	
	if (!bGraphUpToDate)
	{
		BuildCalculationOrder();
	}
	
	for (const FGameplayTag& StatTag : CalculationOrder)
	{
		if (DirtyStats.Contains(StatTag))
		{
			ExecuteStatCalculation(StatTag);
		}
	}
	
	DirtyStats.Empty();
	bIsRecalculating = false;
}

void UEntities_StatComponent::InitializeStat(FGameplayTag StatTag, float BaseValue)
{
	if (EntityTags::IsResourceTag(StatTag))
	{
		return;
	}
	FStatLimits Limits;
	
	if (const UStatConfigDataAsset* Config = GetStatConfig())
	{
		if (const FStatConfig* StatConfig = Config->GetConfig(StatTag))
		{
			Limits.SoftCap = StatConfig->SoftCap;
			Limits.HardCap = StatConfig->HardCap;
			Limits.MinValue = StatConfig->MinValue;
			Limits.bCanExceedSoftCap = StatConfig->bCanExceedSoftCap;
		}
	}
	
	FStatInstance& Instance = StatInstance.FindOrAdd(StatTag);
	Instance.Initialize(BaseValue, Limits);
}

void UEntities_StatComponent::InitializeResource(FGameplayTag StatTag, float MaxValue)
{
	if (!EntityTags::IsResourceTag(StatTag))
	{
		return;
	}
	FStatLimits Limits;

	if (const UStatConfigDataAsset* Config = GetStatConfig())
	{
		if (const FResourceConfig* ResourceConfig = Config->GetResourceConfig(StatTag))
		{
			Limits.SoftCap = ResourceConfig->SoftCap;
			Limits.HardCap = ResourceConfig->HardCap;
			Limits.MinValue = ResourceConfig->MinValue;
			Limits.bCanExceedSoftCap = ResourceConfig->bCanExceedSoftCap;
		}
	}
	
	FResourceInstance& Resource = ResourceInstance.FindOrAdd(StatTag);
	Resource.Initialize(MaxValue, Limits.MinValue);
	Resource.Max.Limits = Limits;
}

void UEntities_StatComponent::ValidateStatInstance()
{
	// TODO: This is nonsense
	/*for (const auto& Pair : StatInstance)
	{
		if (!StatInstance.Contains(Pair.Key))
		{
			InitializeStat(Pair.Key, Pair.Value);
		}
	}*/
}

UEntities_DataAssetMain* UEntities_StatComponent::GetEntityData() const
{	
	if (const UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
	{
		if (EntityAssetId.IsValid())
		{
			return Cast<UEntities_DataAssetMain>(AssetManager->GetPrimaryAssetObject(EntityAssetId));
		}
	}
	return nullptr;	
}

const UStatConfigDataAsset* UEntities_StatComponent::GetStatConfig() const
{
	if (const UWorld* World = GetWorld())
	{
		if (const UGameInstance* GameInst = World->GetGameInstance())
		{
			if (const UStatConfigSubsystem* ConfSys = GameInst->GetSubsystem<UStatConfigSubsystem>())
			{
				return ConfSys->GetStatConfig();
			}
		}
	}
	return nullptr;
}
