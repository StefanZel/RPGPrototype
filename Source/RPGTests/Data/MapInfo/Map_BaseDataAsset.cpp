// Fill out your copyright notice in the Description page of Project Settings.


#include "Map_BaseDataAsset.h"
#include "Engine/AssetManager.h"

void UMap_BaseDataAsset::SetEntitiesWeight()
{
	CalculatedEntityWeight = FMath::RandRange(BaseEntityWeight, BaseEntityWeight + (MapLevel * 1000));
}

void UMap_BaseDataAsset::SetEnemiesToSpawn()
{
	EnemiesToSpawn.Empty();

	if (const UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
	{
		TMap<UEntities_DataAssetMain*, int> AvailableEnemies;
		for (int i = 0; AvailableTypes.Num() > i; i++)
		{
			if (UEntities_DataAssetMain* EntityData = Cast<UEntities_DataAssetMain>(AssetManager->GetPrimaryAssetObject(AvailableTypes[i])))
			{
				AvailableEnemies.Add(EntityData, EntityData->BaseSpawnWeight);
			}
		}
		CalculateWeights(AvailableEnemies);
	}
}

void UMap_BaseDataAsset::CalculateWeights(TMap<UEntities_DataAssetMain*, int> Types)
{
	int CurrWeight = CalculatedEntityWeight;
	int MinWeight = 0;
	int TotalEntityWeight = 0;

	for (const auto& Type : Types)
	{
		if (Type.Value > MinWeight)
		{
			MinWeight = Type.Value;
		}

		TotalEntityWeight += Type.Value;
	}

	if (TotalEntityWeight == 0)
	{
		return;
	}

	while (CurrWeight >= MinWeight)
	{
		int RandomWeight = FMath::RandRange(0, TotalEntityWeight);

		int CumulativeWeight = 0;
		for (const auto& Type : Types)
		{
			CumulativeWeight += Type.Value;
			if (RandomWeight <= CumulativeWeight)
			{
				if (Type.Value <= CurrWeight)
				{
					EnemiesToSpawn.Add(Type.Key);
					CurrWeight -= Type.Value;
				}

				break;
			}
		}
	}
}

