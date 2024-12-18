// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Ai_DataAssetMain.generated.h"

class UBehaviorTree;
class AEntities_AiControllerMain;
/**
 * 
 */
UCLASS()
class RPGTESTS_API UAi_DataAssetMain : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Asset Settings")
	FPrimaryAssetType DataType;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(DataType, GetFName());
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Perception)
	float SightRadius = 1500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Perception)
	float LoseSightRadius = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Perception)
	float VisionAngleDegree = 85.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Perception)
	float SightAge = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Perception)
	bool DetectEnemies = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Perception)
	bool DetectAllies = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Perception)
	bool DetectNeutral = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Perception)
	float AutoSuccessRangeFromLastSeenLocation = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Behavior)
	TSoftObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Behavior)
	TSoftClassPtr<AEntities_AiControllerMain> AIControllerClass;

};
