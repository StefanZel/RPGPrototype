﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities_DataTypes.h"
#include "Engine/DataAsset.h"
#include "Entities_DataAssetMain.generated.h"

/**
 * 
 */
UCLASS()
class RPGTESTS_API UEntities_DataAssetMain : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Asset Settings")
	FPrimaryAssetType DataType;
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override 
	{
	return FPrimaryAssetId(DataType, GetFName());
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Configuration)
	EEntities_AvailableTypes EntityType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Selection)
	TSoftObjectPtr<UMaterialInstance> HighLightMaterial;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Selection)
	TSoftObjectPtr<UMaterialInstance> SelectedMaterial;
};