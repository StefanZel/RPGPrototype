// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameMode_BaseAsset.generated.h"

/**
 * 
 */
UCLASS()
class RPGTESTS_API UGameMode_BaseAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Asset Settings")
	FPrimaryAssetType DataType;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(DataType, GetFName());
	}


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Map, meta = (AllowedTypes="Map"))
	FPrimaryAssetId MapId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Map)
	FText MapName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Display)
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Display)
	FText Desc;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Display)
	TSoftObjectPtr<UTexture2D> Image;
};
