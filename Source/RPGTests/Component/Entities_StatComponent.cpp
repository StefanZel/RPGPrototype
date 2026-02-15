
#include "Entities_StatComponent.h"

#include "Engine/AssetManager.h"
#include "Interfaces/IPluginManager.h"
#include "RPGTests/Data/Entities/Entities_Tags.h"


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
	
	
	for (const FStatScalingRule& Rule : DataAsset->EntityScalingRule)
	{
		if (!StatInstance.Contains(Rule.TargetStat))
		{
			InitializeStat(Rule.TargetStat, 0.f);
		}
	}
	
	RegisterScalingRules(DataAsset);
	
	BuildDependencyGraph();
	BuildCalculationOrder();
	
	for (const FStatModifier& Modifier : DataAsset->StartingModifier)
	{
		AddModifier(Modifier);
	}
	
	RecalculateAllStats();
}

void UEntities_StatComponent::ResetStats()
{
	StatInstance.Empty();
	ActiveModifiers.Empty();
	BaseStatValue.Empty();
	CurrentResourceValues.Empty();
	ModifierIndexMap.Empty();
	CalculationOrder.Empty();
	EntityScalingRules.Empty();
	EntityStateTags.Reset();
	
	bGraphUpToDate = false;
}

float UEntities_StatComponent::GetStatValue(FGameplayTag StatTag)
{
	if (!StatTag.IsValid()) return 0.f;
	
	if (EntityTags::IsResourceCurrentTag(StatTag)) return GetCurrentResource(StatTag);
	
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

float UEntities_StatComponent::GetBaseStatValue(FGameplayTag StatTag) const
{
	if (const float* BaseValue = BaseStatValue.Find(StatTag))
	{
		return *BaseValue;
	}
	return 0.f;
}

void UEntities_StatComponent::SetBaseStatValue(FGameplayTag StatTag, float NewValue)
{
	if (!StatTag.IsValid()) return;
	
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
			BaseStatValue.Add(StatTag, NewValue);
		
			MarkStatAndDependentsDirty(StatTag);
			//RecalculateStatAndDependants(StatTag);
			RecalculateDirtyStats();
		
			OnStatChanged.Broadcast(StatTag, OldValue, NewValue);
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

float UEntities_StatComponent::GetCurrentResource(FGameplayTag ResourceTag) const
{
	if (!EntityTags::IsResourceCurrentTag(ResourceTag))
	{
		if (EntityTags::IsResourceMaxTag(ResourceTag))
		{
			ResourceTag = EntityTags::GetResourceCurrentTag(ResourceTag);
		}
	}
	
	if (const float* Value = CurrentResourceValues.Find(ResourceTag))
	{
		return *Value;
	}
	
	return 0.f;
}

void UEntities_StatComponent::SetCurrentResource(FGameplayTag ResourceTag, float NewValue)
{
	if (!EntityTags::IsResourceCurrentTag(ResourceTag))
	{
		if (EntityTags::IsResourceMaxTag(ResourceTag))
		{
			ResourceTag = EntityTags::GetResourceCurrentTag(ResourceTag);
		}
		else
		{
			return;
		}
	}
	
	
	float OldValue = GetCurrentResource(ResourceTag);
	float MaxValue = GetMaxResource(ResourceTag);
	
	NewValue = FMath::Clamp(NewValue, 0.f, MaxValue);
	
	// Not sure how will this behave, need to test it.
	if (!FMath::IsNearlyEqual(OldValue,NewValue))
	{
		CurrentResourceValues.Add(ResourceTag, NewValue);
		
		if (FStatInstance* Instance = StatInstance.Find(ResourceTag))
		{
			Instance->CurrentValue = NewValue;
		}
		
		OnStatChanged.Broadcast(ResourceTag, OldValue, NewValue);
	}
}

float UEntities_StatComponent::ModifyCurrentResource(FGameplayTag ResourceTag, float Delta)
{
	float Current = GetCurrentResource(ResourceTag);
	float NewValue = Current + Delta;
	SetCurrentResource(ResourceTag, NewValue);
	return GetCurrentResource(ResourceTag);
}

float UEntities_StatComponent::GetMaxResource(FGameplayTag ResourceTag)
{
	FGameplayTag MaxTag;
	
	if (EntityTags::IsResourceCurrentTag(ResourceTag))
	{
		MaxTag = EntityTags::GetResourceMaxTag(ResourceTag);
	}
	else if (EntityTags::IsResourceMaxTag(ResourceTag))
	{
		MaxTag = ResourceTag;
	}
	else
	{
		return 0.f;
	}
	
	return GetStatValue(MaxTag);
}

bool UEntities_StatComponent::HasEnoughResources(FGameplayTag ResourceTag, float Amount) const
{
	return GetCurrentResource(ResourceTag) >= Amount;
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
	
	int32 Index = ActiveModifiers.Add(NewModifier);
	ModifierIndexMap.Add(NewModifier.SourceId, Index);
	
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
	if (int32* IndexPtr = ModifierIndexMap.Find(ModifierId))
	{
		int32 Index = *IndexPtr;
		if (ActiveModifiers.IsValidIndex(Index))
		{
			FActiveModifier Modifier = ActiveModifiers[Index];
			
			ModifierIndexMap.Remove(ModifierId);
			ActiveModifiers.RemoveAt(Index);
			
			for (auto& Pair: ModifierIndexMap)
			{
				if (Pair.Value > Index)
				{
					Pair.Value--;
				}
			}
			
			RemoveModifierInternal(Modifier);
			
			// TODO: Handle duration timer if needed
			OnModifierRemoved.Broadcast(Modifier);
			
			return true;
		}
	}
	
	return false;
}

void UEntities_StatComponent::RemoveModifierBySource(const FName& SourceName)
{
	TArray<FGuid> ModifiersToRemove;
	
	for (const FActiveModifier& Modifier: ActiveModifiers)
	{
		if (Modifier.Data.SourceName == SourceName)
		{
			ModifiersToRemove.Add(Modifier.SourceId);
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
	
	for (const FActiveModifier& Modifier : ActiveModifiers)
	{
		if (Modifier.Data.SourceType == SourceType)
		{
			ModifiersToRemove.Add(Modifier.SourceId);
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
	for (const auto& Pair : ModifierIndexMap)
	{
		AllModifierIds.Add(Pair.Key);
	}
	
	for (const FGuid& Id : AllModifierIds)
	{
		RemoveModifier(Id);
	}
	
}

TArray<FActiveModifier> UEntities_StatComponent::GetModifiersForStat(FGameplayTag StatTag) const
{
	TArray<FActiveModifier> Result;
	
	for (const FActiveModifier& Modifier : ActiveModifiers)
	{
		if (Modifier.Data.TargetStat == StatTag)
		{
			Result.Add(Modifier);
		}
	}
	
	return Result;
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

void UEntities_StatComponent::RecalculateStatAndDependants(FGameplayTag StatTag)
{
	if (!bGraphUpToDate)
	{
		BuildCalculationOrder();
	}
	
	int32 StartIndex = CalculationOrder.IndexOfByKey(StatTag);
	if (StartIndex != INDEX_NONE)
	{
		bIsRecalculating = true;
		
		for (int32 i = StartIndex; i < CalculationOrder.Num(); i++)
		{
			ExecuteStatCalculation(CalculationOrder[i]);
		}
		
		bIsRecalculating = false;
	}
	else
	{
		RecalculateAllStats();
	}
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
        if (EntityTags::IsResourceCurrentTag(Tag)) continue;
        
        float FinalValue = GetStatValue(Tag);
        float BaseVal = BaseStatValue.FindRef(Tag);
        FGameplayTag Category = EntityTags::GetStatCategory(Tag);
        
        bool bIsMaxResource = EntityTags::IsResourceMaxTag(Tag);
        
        UE_LOG(LogTemp, Log, TEXT("[%s] %s: %.2f (Base: %.2f) %s%s"), 
            *Category.GetTagName().ToString().Replace(TEXT("Stat."), TEXT("")), 
            *Tag.GetTagName().ToString(), 
            FinalValue, 
            BaseVal,
            bIsMaxResource ? TEXT("[MAX RESOURCE] ") : TEXT(""),
            !FMath::IsNearlyEqual(FinalValue, BaseVal) ? TEXT("*MODIFIED*") : TEXT(""));
    }

    UE_LOG(LogTemp, Warning, TEXT("--- Resources ---"));
    
    TSet<FGameplayTag> ResourceBaseTags;
    for (const auto& Pair : CurrentResourceValues)
    {
        FGameplayTag BaseTag = EntityTags::GetResourceBaseTag(Pair.Key);
        if (BaseTag.IsValid())
        {
            ResourceBaseTags.Add(BaseTag);
        }
    }
    
    for (FGameplayTag BaseResource : ResourceBaseTags)
    {
        FGameplayTag CurrentTag = FGameplayTag::EmptyTag;
        FGameplayTag MaxTag = FGameplayTag::EmptyTag;
        
        FString BaseString = BaseResource.ToString();
        FString CurrentString = BaseString + TEXT(".Current");
        FString MaxString = BaseString + TEXT(".Max");
        
        CurrentTag = FGameplayTag::RequestGameplayTag(FName(*CurrentString));
        MaxTag = FGameplayTag::RequestGameplayTag(FName(*MaxString));
        
        if (CurrentTag.IsValid() && MaxTag.IsValid())
        {
            float CurrentValue = GetCurrentResource(CurrentTag);
            float MaxValue = GetStatValue(MaxTag); 
            
            UE_LOG(LogTemp, Log, TEXT("%s: %.1f / %.1f (%.0f%%)"), 
                *BaseResource.ToString(),
                CurrentValue, 
                MaxValue,
                MaxValue > 0 ? (CurrentValue / MaxValue * 100) : 0.f);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("--- Active Modifiers: %d ---"), ActiveModifiers.Num());
    for (const FActiveModifier& Mod : ActiveModifiers)
    {
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
	
	for (const auto& StatPair : DataAsset->BaseStats)
	{
		FGameplayTag StatTag = StatPair.Key;
		float BaseValue =  StatPair.Value;
		
		if (EntityTags::IsResourceTag(StatTag))
		{
			if (EntityTags::IsResourceBaseTag(StatTag))
			{
				FGameplayTag MaxTag = EntityTags::GetResourceMaxTagFromBase(StatTag);
				FGameplayTag CurrentTag = EntityTags::GetResourceCurrentTagFromBase(StatTag);
				
				InitializeStat(MaxTag, BaseValue, 0.f, 0.f, true);
				CurrentResourceValues.Add(CurrentTag, BaseValue);
			}
			else if (EntityTags::IsResourceMaxTag(StatTag))
			{
				InitializeStat(StatTag, BaseValue);	
				
				FGameplayTag CurrentTag = EntityTags::GetResourceCurrentTag(StatTag);
				CurrentResourceValues.Add(CurrentTag, BaseValue);
			}
			else if (EntityTags::IsResourceCurrentTag(StatTag))
			{
				CurrentResourceValues.Add(StatTag, BaseValue);
				
				FGameplayTag MaxTag = EntityTags::GetResourceMaxTag(StatTag);
				InitializeStat(MaxTag, BaseValue);
			}
		}
		InitializeStat(StatTag, BaseValue);
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

void UEntities_StatComponent::BuildDependencyGraph()
{
	DependencyGraph.Empty();
	
	for (const auto& Pair : StatInstance)
	{
		FGameplayTag StatTag = Pair.Key;
		if (EntityTags::IsResourceCurrentTag(StatTag))
		{
			FGameplayTag MaxTag = EntityTags::GetResourceMaxTag(StatTag);
			if (MaxTag.IsValid())
			{
				AddDependency(MaxTag, StatTag);
			}
		}
	}
	
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
	
	for (const auto& Pair : BaseStatValue)
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
	
	while (Queue.Num() > 0)
	{
		FGameplayTag Current = Queue[0];
		Queue.RemoveAt(0);
		
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
	
	FStatInstance* Instance = StatInstance.Find(StatTag);
	if (!Instance)
	{
		InitializeStat(StatTag, 0.f);
		Instance = StatInstance.Find(StatTag);
		if (!Instance) return;
	}
	
	FModifierBuckets Buckets;
	Buckets.Base = BaseStatValue.FindRef(StatTag);
	
	if (const FStatDependencyNode* Node = DependencyGraph.Find(StatTag))
	{
		for (const FStatScalingRule& Rule : Node->ScalingRules)
		{
			float SourceValue = GetStatValue(Rule.SourceStat);
			float Bonus = Rule.BaseValue + (SourceValue * Rule.ScalePerUnit);
			
			ApplyModifierToBucket(Buckets, Rule.ModifierType, Bonus);
		}
	}
	
	GatherModifiersFromStat(StatTag, Buckets);
	
	float OldValue = Instance->CurrentValue;
	
	Instance->Buckets = Buckets;
	Instance->Recalculate();
	
	float NewValue = Instance->CurrentValue;

	if (EntityTags::IsResourceMaxTag(StatTag))
	{
	RecalculateCurrentStat(StatTag, NewValue, OldValue);
	}
	
	if (!FMath::IsNearlyEqual(OldValue, NewValue))
	{
		OnStatChanged.Broadcast(StatTag, OldValue, NewValue);
	}
	
}

/*void UEntities_StatComponent::RecalculateStatFrom(FGameplayTag StartStatTag)
{
	if (!bGraphUpToDate)
	{
		BuildCalculationOrder();
	}
	
	int32 StartIndex = CalculationOrder.IndexOfByKey(StartStatTag);
	if (StartIndex != INDEX_NONE)
	{
		bIsRecalculating = true;
		for (int32 i = StartIndex; i < CalculationOrder.Num(); i++)
		{
			ExecuteStatCalculation(CalculationOrder[i]);
		}
		bIsRecalculating = false;
	}
	else
	{
		RecalculateAllStats();
	}
}*/

void UEntities_StatComponent::GatherModifiersFromStat(FGameplayTag StatTag, FModifierBuckets& OutBuckets)
{
	if (EntityTags::IsResourceCurrentTag(StatTag)) return;
	
	TMap<EStatModifierType, TArray<FStatModifier>> ModifiersByType;
	TMap<FGameplayTag, float> ConvertedValues;
	
	for (const FActiveModifier& Modifier : ActiveModifiers)
	{
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

void UEntities_StatComponent::RecalculateCurrentStat(const FGameplayTag& StatTag, float NewValue, float OldMax)
{
	if (!EntityTags::IsResourceMaxTag(StatTag)) return;
	
	FGameplayTag CurrentTag = EntityTags::GetResourceCurrentTag(StatTag);
	
	float* CurrentValue = CurrentResourceValues.Find(CurrentTag);
	if (!CurrentValue) return;

	if (OldMax <= 0.f || FMath::IsNearlyZero(OldMax))
	{
		*CurrentValue = NewValue;
	}
	else
	{
		float Ratio = FMath::Clamp(*CurrentValue / OldMax, 0.f, 1.f);
		*CurrentValue = FMath::Clamp(Ratio * NewValue, 0.f, NewValue);
	}
	
	if (FStatInstance* CurrentInstance = StatInstance.Find(CurrentTag))
	{
		CurrentInstance->CurrentValue = *CurrentValue;
	}

	
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
	RecalculateDirtyStats();
	//RecalculateStatFrom(Modifier.Data.TargetStat);
}

void UEntities_StatComponent::RemoveModifierInternal(const FActiveModifier& Modifier)
{
	MarkStatAndDependentsDirty(Modifier.Data.TargetStat);
	RecalculateDirtyStats();
	//RecalculateStatFrom(Modifier.Data.TargetStat);
}

void UEntities_StatComponent::OnModifierExpired(FGuid ModifierId)
{
	RemoveModifier(ModifierId);
}

void UEntities_StatComponent::RecalculateConditionalModifiers()
{
	bool bAnyChanged = false;
	
	for (FActiveModifier& Modifier : ActiveModifiers)
	{
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
	
	while (ToProcess.Num() > 0)
	{
		FGameplayTag Current = ToProcess[0];
		ToProcess.RemoveAt(0);
		
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

void UEntities_StatComponent::InitializeStat(FGameplayTag StatTag, float BaseValue, float MinValue, float MaxValue,
                                             bool bIsResource)
{
	if (EntityTags::IsResourceCurrentTag(StatTag))
	{
		return;
	}
	if (EntityTags::IsResourceMaxTag(StatTag))
	{
		bIsResource = true;
	}
	else if (EntityTags::IsResourceBaseTag(StatTag))
	{
		bIsResource = true;
		StatTag = EntityTags::GetResourceMaxTagFromBase(StatTag);
	}
	
	BaseStatValue.Add(StatTag, BaseValue);
	
	FStatInstance& Instance = StatInstance.FindOrAdd(StatTag);
	Instance.Initialize(BaseValue, MinValue, MaxValue, bIsResource);
	
	if (bIsResource)
	{
		Instance.LinkedBaseStat = EntityTags::GetResourceBaseTag(StatTag);
		Instance.LinkedCurrentStat = EntityTags::GetResourceCurrentTag(StatTag);
	}
	
}

void UEntities_StatComponent::ValidateStatInstance()
{
	for (const auto& Pair : BaseStatValue)
	{
		if (!StatInstance.Contains(Pair.Key))
		{
			InitializeStat(Pair.Key, Pair.Value);
		}
	}
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
