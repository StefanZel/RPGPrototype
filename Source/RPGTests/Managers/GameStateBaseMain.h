// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RPGTests/Data/GameMode/GameMode_DataTypes.h"
#include "GameStateBaseMain.generated.h"

class UGameMode_BaseAsset;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGamePhaseChangedDelegate, const EGamePhaseState);
DECLARE_MULTICAST_DELEGATE(FOnGamePhaseLoadGameDataDelegate);
DECLARE_MULTICAST_DELEGATE(FOnGamePhaseInitiateDelegate);
DECLARE_MULTICAST_DELEGATE(FOnGamePhaseSetupDelegate);
DECLARE_MULTICAST_DELEGATE(FOnGamePhasePreLoadDelegate);
DECLARE_MULTICAST_DELEGATE(FOnGamePhasePreGameplayDelegate);
DECLARE_MULTICAST_DELEGATE(FOnGamePhaseGameplayDelegate);
DECLARE_MULTICAST_DELEGATE(FOnGamePhasePostGameplayDelegate);
DECLARE_MULTICAST_DELEGATE(FOnGamePhaseExitDelegate);
/**
 * 
 */
UCLASS()
class RPGTESTS_API AGameStateBaseMain : public AGameStateBase
{
	GENERATED_BODY()

public:
	AGameStateBaseMain(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaSeconds) override;
	void SetGameData(UGameMode_BaseAsset* NewGameData);
	EGamePhaseState GetGamePhaseState() const {return GamePhaseState;}
	void ResetGameStateRegistry();
	void RegisterPhaseTaskComplete(const FGuid RegistrationId);
	void OnGameplay();
	FOnGamePhaseChangedDelegate& OnGamePhaseChangedChecked();

	void CallOrRegisterPhase_OnGamePhaseLoadGameData(FOnGamePhaseLoadGameDataDelegate::FDelegate&& Delegate, FGuid& TaskId, const FString& DebugName = FString());
	void CallOrRegisterPhase_OnGamePhaseInitiate(FOnGamePhaseInitiateDelegate::FDelegate&& Delegate, FGuid& TaskId, const FString& DebugName = FString());
	void CallOrRegisterPhase_OnGamePhaseSetup(FOnGamePhaseSetupDelegate::FDelegate&& Delegate, FGuid& TaskId, const FString& DebugName = FString());
	void CallOrRegisterPhase_OnGamePhasePreLoad(FOnGamePhasePreLoadDelegate::FDelegate&& Delegate, FGuid& TaskId, const FString& DebugName = FString());
	void CallOrRegisterPhase_OnGamePhasePreGameplay(FOnGamePhasePreGameplayDelegate::FDelegate&& Delegate, FGuid& TaskId, const FString& DebugName = FString());
	void CallOrRegisterPhase_OnGamePhaseGameplay(FOnGamePhaseGameplayDelegate::FDelegate&& Delegate, FGuid& TaskId, const FString& DebugName = FString());
	void CallOrRegisterPhase_OnGamePhasePostGameplay(FOnGamePhasePostGameplayDelegate::FDelegate&& Delegate, FGuid& TaskId, const FString& DebugName = FString());
	void CallOrRegisterPhase_OnGamePhaseExit(FOnGamePhaseExitDelegate::FDelegate&& Delegate, FGuid& TaskId, const FString& DebugName = FString());

protected:
	
	virtual void BeginPlay() override;
	void OnGameDataLoaded();
	void StartGamePhaseTimer();
	void GamePhaseTimer();
	bool ShouldStartNextPhase();
	void StartNextPhase();
	void SetGamePhaseState(const EGamePhaseState NewGamePhase);
	void RegisterGamePhase(const EGamePhaseState Phase, FGuid& TaskId, const FString& DebugName);
	
	
	UPROPERTY()
	UGameMode_BaseAsset* GameData;

	UPROPERTY()
	EGamePhaseState GamePhaseState = EGamePhaseState::Loading;

	UPROPERTY()
	TMap<FGuid, FPhaseRegistrationData> PhaseRegistrationsData;

	FOnGamePhaseChangedDelegate OnGamePhaseChanged;
	FOnGamePhaseLoadGameDataDelegate OnGamePhaseLoadGameData;
	FOnGamePhaseInitiateDelegate OnGamePhaseInitiate;
	FOnGamePhaseSetupDelegate OnGamePhaseSetup;
	FOnGamePhasePreLoadDelegate OnGamePhasePreLoad;
	FOnGamePhasePreGameplayDelegate OnGamePhasePreGameplay;
	FOnGamePhaseGameplayDelegate OnGamePhaseGameplay;
	FOnGamePhasePostGameplayDelegate OnGamePhasePostGameplay;
	FOnGamePhaseExitDelegate OnGamePhaseExit;
	
private:

	UPROPERTY()
	FTimerHandle TimerHandle_PhaseTimer;

	UPROPERTY()
	FGuid WaitGameDataTaskId;

	UPROPERTY()
	FGuid GameplayTaskId;
};
