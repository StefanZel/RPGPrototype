// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStateBaseMain.h"

AGameStateBaseMain::AGameStateBaseMain(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	GameData = nullptr;
}

void AGameStateBaseMain::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Blue, FString::Printf(TEXT("%s"), *UEnum::GetDisplayValueAsText(GamePhaseState).ToString()));
	}
}

void AGameStateBaseMain::SetGameData(UGameMode_BaseAsset* NewGameData)
{
	if (NewGameData != nullptr)
	{
		GameData = NewGameData;
		OnGameDataLoaded();

	}
}
void AGameStateBaseMain::ResetGameStateRegistry()
{
	PhaseRegistrationsData.Empty();
}

void AGameStateBaseMain::RegisterPhaseTaskComplete(const FGuid RegistrationId)
{
	if (FPhaseRegistrationData* DataPtr = PhaseRegistrationsData.Find(RegistrationId))
	{
		DataPtr->bIsCompleted = true;
	}
}
void AGameStateBaseMain::BeginPlay()
{
	Super::BeginPlay();

	CallOrRegisterPhase_OnGamePhaseLoadGameData(FOnGamePhaseLoadGameDataDelegate::FDelegate::CreateUObject(this, &ThisClass::OnGameDataLoaded), 
		WaitGameDataTaskId, 
		FString("State - WaitForGameData"));
	CallOrRegisterPhase_OnGamePhaseGameplay(FOnGamePhaseGameplayDelegate::FDelegate::CreateUObject(this, &ThisClass::OnGameplay), 
		GameplayTaskId, 
		FString("State - Gameplay"));

	StartGamePhaseTimer();
}

void AGameStateBaseMain::OnGameDataLoaded()
{
	if (GameData != nullptr)
	{
		if (WaitGameDataTaskId.IsValid())
		{
			RegisterPhaseTaskComplete(WaitGameDataTaskId);
		}
	}
}

void AGameStateBaseMain::OnGameplay()
{

}

FOnGamePhaseChangedDelegate& AGameStateBaseMain::OnGamePhaseChangedChecked()
{
	FOnGamePhaseChangedDelegate* Result = &OnGamePhaseChanged;
	check(Result);
	return *Result;
}

void AGameStateBaseMain::StartGamePhaseTimer()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_PhaseTimer, this, &ThisClass::GamePhaseTimer, 1.f, true, 1.f);
	}
}

void AGameStateBaseMain::GamePhaseTimer()
{
	if (ShouldStartNextPhase())
	{
		StartNextPhase();
	}
}

bool AGameStateBaseMain::ShouldStartNextPhase()
{
	for (auto I = PhaseRegistrationsData.CreateConstIterator(); I; ++I)
	{
		if (I.Value().GamePhase == GetGamePhaseState())
		{
			if (!I.Value().bIsCompleted)
			{
				return false;
			}
		}
	}
	return true;
}

void AGameStateBaseMain::StartNextPhase()
{
	const int32 NextPhaseIndex = static_cast<int32>(GetGamePhaseState()) + 1;

	// TODO: Change this later for level transition and such...
	if (NextPhaseIndex >= static_cast<int32>(EGamePhaseState::Exit))
	{
		SetGamePhaseState(EGamePhaseState::GenerateNewGameData);
	}
	else
	{
		SetGamePhaseState(static_cast<EGamePhaseState>(NextPhaseIndex));
	}
}

void AGameStateBaseMain::SetGamePhaseState(const EGamePhaseState NewGamePhase)
{
	if (NewGamePhase != GetGamePhaseState())
	{
		GamePhaseState = NewGamePhase;

		OnGamePhaseChanged.Broadcast(GamePhaseState);

		switch (GamePhaseState) 
		{
		case EGamePhaseState::Loading:
			break;
		case EGamePhaseState::GenerateNewGameData:
			break;
		case EGamePhaseState::LoadGameData:
			OnGamePhaseLoadGameData.Broadcast();
			break;
		case EGamePhaseState::Initiate:
			OnGamePhaseInitiate.Broadcast();
			break;
		case EGamePhaseState::Setup:
			OnGamePhaseSetup.Broadcast();
			break;
		case EGamePhaseState::PreLoad:
			OnGamePhasePreLoad.Broadcast();
			break;
		case EGamePhaseState::PreGameplay:
			OnGamePhasePreGameplay.Broadcast();
			break;
		case EGamePhaseState::Gameplay:
			OnGamePhaseGameplay.Broadcast();
			break;
		case EGamePhaseState::PostGameplay:
			OnGamePhasePostGameplay.Broadcast();
			break;
		case EGamePhaseState::Exit:
			OnGamePhaseExit.Broadcast();
			break;
		}
	}
}

void AGameStateBaseMain::RegisterGamePhase(const EGamePhaseState Phase, FGuid& TaskId, const FString& DebugName)
{
	if (!TaskId.IsValid())
	{
		TaskId = FGuid::NewGuid();
	}

	PhaseRegistrationsData.Add(TaskId, FPhaseRegistrationData(Phase, DebugName));
}

void AGameStateBaseMain::CallOrRegisterPhase_OnGamePhaseLoadGameData(
	FOnGamePhaseLoadGameDataDelegate::FDelegate&& Delegate, FGuid& TaskId, const FString& DebugName)
{
	if(GetGamePhaseState() >= EGamePhaseState::LoadGameData)
	{
		Delegate.Execute();
	}
	else
	{
		RegisterGamePhase(EGamePhaseState::LoadGameData, TaskId, DebugName);
		OnGamePhaseLoadGameData.Add(MoveTemp(Delegate));
	}
}

void AGameStateBaseMain::CallOrRegisterPhase_OnGamePhaseInitiate(FOnGamePhaseInitiateDelegate::FDelegate&& Delegate,
	FGuid& TaskId, const FString& DebugName)
{
	if(GetGamePhaseState() >= EGamePhaseState::Initiate)
	{
		Delegate.Execute();
	}
	else
	{
		RegisterGamePhase(EGamePhaseState::Initiate, TaskId, DebugName);
		OnGamePhaseInitiate.Add(MoveTemp(Delegate));
	}
}

void AGameStateBaseMain::CallOrRegisterPhase_OnGamePhaseSetup(FOnGamePhaseSetupDelegate::FDelegate&& Delegate,
	FGuid& TaskId, const FString& DebugName)
{
	if(GetGamePhaseState() >= EGamePhaseState::Setup)
	{
		Delegate.Execute();
	}
	else
	{
		RegisterGamePhase(EGamePhaseState::Setup, TaskId, DebugName);
		OnGamePhaseSetup.Add(MoveTemp(Delegate));
	}
}

void AGameStateBaseMain::CallOrRegisterPhase_OnGamePhasePreLoad(FOnGamePhasePreLoadDelegate::FDelegate&& Delegate,
	FGuid& TaskId, const FString& DebugName)
{
	if(GetGamePhaseState() >= EGamePhaseState::PreLoad)
	{
		Delegate.Execute();
	}
	else
	{
		RegisterGamePhase(EGamePhaseState::PreLoad, TaskId, DebugName);
		OnGamePhasePreLoad.Add(MoveTemp(Delegate));
	}
}

void AGameStateBaseMain::CallOrRegisterPhase_OnGamePhasePreGameplay(
	FOnGamePhasePreGameplayDelegate::FDelegate&& Delegate, FGuid& TaskId, const FString& DebugName)
{
	if(GetGamePhaseState() >= EGamePhaseState::PreGameplay)
	{
		Delegate.Execute();
	}
	else
	{
		RegisterGamePhase(EGamePhaseState::PreGameplay, TaskId, DebugName);
		OnGamePhasePreGameplay.Add(MoveTemp(Delegate));
	}
}

void AGameStateBaseMain::CallOrRegisterPhase_OnGamePhaseGameplay(FOnGamePhaseGameplayDelegate::FDelegate&& Delegate,
	FGuid& TaskId, const FString& DebugName)
{
	if(GetGamePhaseState() >= EGamePhaseState::Gameplay)
	{
		Delegate.Execute();
	}
	else
	{
		RegisterGamePhase(EGamePhaseState::Gameplay, TaskId, DebugName);
		OnGamePhaseGameplay.Add(MoveTemp(Delegate));
	}
}

void AGameStateBaseMain::CallOrRegisterPhase_OnGamePhasePostGameplay(
	FOnGamePhasePostGameplayDelegate::FDelegate&& Delegate, FGuid& TaskId, const FString& DebugName)
{
	if(GetGamePhaseState() >= EGamePhaseState::PostGameplay)
	{
		Delegate.Execute();
	}
	else
	{
		RegisterGamePhase(EGamePhaseState::PostGameplay, TaskId, DebugName);
		OnGamePhasePostGameplay.Add(MoveTemp(Delegate));
	}
}

void AGameStateBaseMain::CallOrRegisterPhase_OnGamePhaseExit(FOnGamePhaseExitDelegate::FDelegate&& Delegate,
	FGuid& TaskId, const FString& DebugName)
{
	if(GetGamePhaseState() >= EGamePhaseState::Exit)
	{
		Delegate.Execute();
	}
	else
	{
		RegisterGamePhase(EGamePhaseState::Exit, TaskId, DebugName);
		OnGamePhaseExit.Add(MoveTemp(Delegate));
	}
}

