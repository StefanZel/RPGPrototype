// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RPGTests/Data/Ai/Ai_DataAssetMain.h"
#include "Entities_AiControllerMain.generated.h"

class UAISenseConfig_Sight;
/**
 * 
 */
UCLASS()
class RPGTESTS_API AEntities_AiControllerMain : public AAIController
{
	GENERATED_BODY()
	
public:

	AEntities_AiControllerMain(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	//virtual void OnPossess(APawn* InPawn) override;
	virtual void InitAiOnGameplay();

	/* Ai Data */
public:
	virtual void SetAiData(const FPrimaryAssetId& InAiDataAssetId) { AiDataAssetId = InAiDataAssetId; }
	UAi_DataAssetMain* GetAiData() const;

protected:

	UPROPERTY()
	FPrimaryAssetId AiDataAssetId;
	/* End Ai Data*/

	/* Behavior */
protected:
	void InitBehavior();

	UPROPERTY()
	UBehaviorTree* BehaviorTreeAsset;

public:

	/* End Behavior */

	/* Perception */
protected:
	void InitPerception();
	void InitSightPerception();
	void CreateStimuliSourceComponent() const;
	void StartUpdatingPerception();
	void EndUpdatingPerception();
	void UpdatePerception();
	bool HasLineOfSight(const AActor* Target) const;

	UFUNCTION()
	void UpdatePercievedActors(const TArray<AActor*>& UpdatedActors);

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY()
	TMap<AActor*, float> PercievedEntities;

	UPROPERTY()
	FTimerHandle Handle_PerceptionUpdate;

	/* End Perception */
};
