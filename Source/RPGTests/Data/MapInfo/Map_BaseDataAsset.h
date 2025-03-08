// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RPGTests/Data/Entities/Entities_DataTypes.h"
#include "RPGTests/Data/Entities/Entities_DataAssetMain.h"
#include "Map_BaseDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class RPGTESTS_API UMap_BaseDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MapData)
	int MapLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MapData)
	int BaseEntityWeight = 1000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MapData)
	int CalculatedEntityWeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MapData)
	TArray<FPrimaryAssetId> AvailableTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MapData)
	TArray<UEntities_DataAssetMain*> EnemiesToSpawn;

	void SetEntitiesWeight();
	void SetEnemiesToSpawn();
	void CalculateWeights(TMap<UEntities_DataAssetMain*, int> Types);
};
