// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities_AbilityComponent.h"
#include "RPGTests/Interfaces/Abilities_Interface.h"
#include "Engine/AssetManager.h"

UEntities_AbilityComponent::UEntities_AbilityComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UEntities_AbilityComponent::Initialize(const FPrimaryAssetId& NewEntityDataAsset, const int32 NewEntityIndex)
{
	Super::Initialize(NewEntityDataAsset, NewEntityIndex);

	SetAbilityDataAssets();
}

void UEntities_AbilityComponent::ActivateAbility(const FPrimaryAssetId& Ability)
{
	SelectedAbility = Ability;

	if (IsAbilityValid())
	{
		UAbilities_NormalDataAsset* AbilityAsset = GetAbilityData();
		AActor* SpawnedAbilityClass = GetWorld()->SpawnActor<AActor>(AbilityAsset->AbilityClass.LoadSynchronous());

		if (IAbilities_Interface* AbilityInitializer = Cast<IAbilities_Interface>(SpawnedAbilityClass))
		{
			AbilityInitializer->InitializeAbility(SelectedAbility);
			ActiveAbility = SpawnedAbilityClass;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to initialize ability."));
		}

	}
}

void UEntities_AbilityComponent::UpdateAbilityPosition(const FVector& Position)
{
	if (!ActiveAbility) return;

	if (IAbilities_Interface* AbilityInterface = Cast<IAbilities_Interface>(ActiveAbility))
	{
		AbilityInterface->UpdateAbility(Position);
	}
}

void UEntities_AbilityComponent::ExecuteAbility()
{
	if (!ActiveAbility) return;

	ActiveAbility->Destroy();
	ActiveAbility = nullptr;
	SelectedAbility = FPrimaryAssetId();
}

FPrimaryAssetId UEntities_AbilityComponent::GetAbilityBySlot(int32 AbilitySlot)
{
	if (AvailableAbilityDataAssets.IsValidIndex(AbilitySlot))
	{
		return AvailableAbilityDataAssets[AbilitySlot];
	}
	return FPrimaryAssetId();
}

bool UEntities_AbilityComponent::IsAbilityValid() const
{
	if (AvailableAbilityDataAssets.Contains(SelectedAbility) || SelectedAbility.IsValid())
	{
		return true;
	}

	return false;
}

void UEntities_AbilityComponent::SetAbilityDataAssets()
{
	if (const UEntities_DataAssetMain* Data = GetData())
	{
		AvailableAbilityDataAssets.Append(Data->Abilities);
	}
}

UAbilities_NormalDataAsset* UEntities_AbilityComponent::GetAbilityData() const
{
	const UAssetManager* AssetManager = UAssetManager::GetIfInitialized();

	if (IsAbilityValid() || AssetManager)
	{
		return Cast<UAbilities_NormalDataAsset>(AssetManager->GetPrimaryAssetObject(SelectedAbility));
	}
	else
	{
		return nullptr;
	}

}

float UEntities_AbilityComponent::GetAbilityRange()
{
	float DefaultRange = 1.f;

	if (const UAbilities_NormalDataAsset* Data = GetAbilityData())
	{
		return Data->XRange;
	}
	return DefaultRange;
}
