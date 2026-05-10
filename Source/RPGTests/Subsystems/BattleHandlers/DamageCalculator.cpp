// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageCalculator.h"

#include "RPGTests/Commands/CommandBase.h"
#include "RPGTests/Component/Entities_StatComponent.h"

void UDamageCalculator::Reset()
{
	PendingDamages.Reset();
}

void UDamageCalculator::CalculateAndQueue(const UCommandBase* Command)
{
	if (!Command || !Command->Data.SourceActor) return;
	if (!Command->Data.HasAbility()) return;
	
	for (const auto& Targets : Command->Data.TargetActors)
	{
		const float Raw = GetBaseDamage(Command->Data.SourceActor);
		const float Calculated = ApplyMitigation(Raw, Targets);
		
		FPendingDamage& Entry = PendingDamages.AddDefaulted_GetRef();
		Entry.Source = Command->Data.SourceActor;
		Entry.Target = Targets;
		Entry.BaseDamage = Calculated;
		Entry.ModifiedDamage = Calculated;
	}
	
}

FPendingDamage* UDamageCalculator::FindPendingForTarget(const AActor* Target)
{
	return PendingDamages.FindByPredicate(
		[Target](const FPendingDamage& Entry) { return Entry.Target == Target; });
}

void UDamageCalculator::CommitAll()
{
	for (const FPendingDamage& Entry : PendingDamages)
	{
		if (Entry.bNegated || !Entry.Target) continue;
		
		if (UEntities_StatComponent* Stats = UEntities_StatComponent::FindEntityStatComponent(Entry.Target))
		{
			Stats->ModifyCurrentResource(EntityTags::Stat::Resources::Health, Entry.ModifiedDamage);
		}
	}
}

float UDamageCalculator::GetBaseDamage(const AActor* Attacker) const
{
	if (const UEntities_StatComponent* Stats = UEntities_StatComponent::FindEntityStatComponent(Attacker))
	{
		return Stats->GetStatInstance(EntityTags::Stat::Offenses::AttackDamage).CurrentValue;
	}
	return 10.f;
}

float UDamageCalculator::ApplyMitigation(const float RawDamage, const AActor* Defender) const
{
	if (const UEntities_StatComponent* Stats = UEntities_StatComponent::FindEntityStatComponent(Defender))
	{
		const float Defense =  Stats->GetStatInstance(EntityTags::Stat::Defenses::Armor).CurrentValue;
		return FMath::Max(0.f, RawDamage - Defense);
	}
	return RawDamage;
}
