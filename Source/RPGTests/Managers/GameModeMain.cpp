


#include "GameModeMain.h"
#include "GameStateBaseMain.h"
#include "RPGTests/Data/AStaticGameData.h"
#include "RPGTests/Data/GameMode/GameMode_BaseAsset.h"
#include "Engine/AssetManager.h"


AGameModeMain::AGameModeMain(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void AGameModeMain::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

void AGameModeMain::InitGameState()
{
	Super::InitGameState();

	if (AGameStateBaseMain* MainGameState = Cast<AGameStateBaseMain>(GameState))
	{
		MainGameState->ResetGameStateRegistry();
		MainGameState->CallOrRegisterPhase_OnGamePhaseLoadGameData(FOnGamePhaseLoadGameDataDelegate::FDelegate::CreateUObject(this, &ThisClass::InitGameData), LoadGameDataTaskId, FString("GameMode - LoadGameData"));

	}

}

void AGameModeMain::InitGameData()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleGameDataAssignment);
	}
}

void AGameModeMain::HandleGameDataAssignment()
{
	if (!CurrentGameDataAssetID.IsValid())
	{
		CurrentGameDataAssetID = FPrimaryAssetId(DATA_ASSET_TYPE_GAMEMODE, DATA_ASSET_TYPE_GAMEMODE_TEST);
	}

	if (CurrentGameDataAssetID.IsValid())
	{
		OnGameDataAssigned();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Failed to identify game data."), *GetNameSafe(this));
	}
}

void AGameModeMain::OnGameDataAssigned()
{
	if (UAssetManager* AssetManager = UAssetManager::GetIfInitialized()) 
	{
		const TArray<FName> Bundles;
		const FStreamableDelegate DataLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnGameDataLoaded);
		AssetManager->LoadPrimaryAsset(CurrentGameDataAssetID, Bundles, DataLoadedDelegate);
	}
}

void AGameModeMain::OnGameDataLoaded()
{
	if (GameState && CurrentGameDataAssetID.IsValid())
	{
		if (AGameStateBaseMain* DefGameState = Cast<AGameStateBaseMain>(GameState))
		{
			if (UGameMode_BaseAsset* GameData = GetGameData())
			{
				DefGameState->SetGameData(GameData);
				DefGameState->RegisterPhaseTaskComplete(LoadGameDataTaskId);
			}
		}
	}
}

UGameMode_BaseAsset* AGameModeMain::GetGameData()
{
	if (CurrentGameDataAssetID.IsValid())
	{
		if (const UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
		{
			return Cast<UGameMode_BaseAsset>(AssetManager->GetPrimaryAssetObject(CurrentGameDataAssetID));
		}
	}

	return nullptr;
}
