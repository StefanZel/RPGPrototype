
#include "GameModeMain.h"
#include "GameStateBaseMain.h"
#include "RPGTests/Data/AStaticGameData.h"
#include "RPGTests/Data/GameMode/GameMode_BaseAsset.h"
#include "Engine/AssetManager.h"
#include "Engine/World.h"
#include "PlayerStartBase.h"
#include "EngineUtils.h"
#include "ControllerMain.h"
#include "SettingsMain.h"
#include "Kismet/GameplayStatics.h"
#include "RPGTests/Component/Entities_Component.h"
#include "RPGTests/Component/Entities_DecalComponent.h"
#include "RPGTests/Data/Ai/Ai_DataAssetMain.h"
#include "RPGTests/Ai/Entities_AiControllerMain.h"
#include "RPGTests/Ai/Entities_AiControllerCommand.h"
#include "GameFramework/Controller.h"


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
		MainGameState->CallOrRegisterPhase_OnGamePhaseLoadGameData(FOnGamePhaseLoadGameDataDelegate::FDelegate::CreateUObject(this, &ThisClass::InitGameData),
			LoadGameDataTaskId, 
			FString("GameMode - LoadGameData"));

	}

}

void AGameModeMain::BeginPlay()
{
	Super::BeginPlay();


	MainController = Cast<AControllerMain>(GetWorld()->GetFirstPlayerController());
}

AActor* AGameModeMain::ChoosePlayerStart_Implementation(AController* Player)
{
	if (GetWorld())
	{
		for (TActorIterator<APlayerStartBase> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			APlayerStartBase* PlayerStart = *ActorItr;
			if (PlayerStart && PlayerStart->TryClaim(Player))
			{
				if (MainController)
				{
					const FVector StartLocation = PlayerStart->GetActorLocation();
					SetPlayerStartLocation(StartLocation);
				}
				return PlayerStart;
			}

		}
	}

	return Super::ChoosePlayerStart_Implementation(Player);
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
		if (UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
		{
			TArray<FPrimaryAssetId> EntityDataAsset;
			if(UGameMode_BaseAsset* CurrentGameData = Cast<UGameMode_BaseAsset>(AssetManager->GetPrimaryAssetObject(CurrentGameDataAssetID)))
			{
				EntityDataAsset.Append(CurrentGameData->Entities);
			}
			
			if(EntityDataAsset.Num()>0)
			{
				const TArray<FName> Bundles;
				const FStreamableDelegate DataLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnEntityDataLoaded, EntityDataAsset);
				AssetManager->LoadPrimaryAssets(EntityDataAsset, Bundles, DataLoadedDelegate);
			}
		}
	}
}

void AGameModeMain::OnEntityDataLoaded(TArray<FPrimaryAssetId> EntityDataAsset)
{

	if (UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
	{
		TArray<FPrimaryAssetId> AiDataAsset;
		for (int i = 0; i < EntityDataAsset.Num(); i++)
		{
			if (const UEntities_DataAssetMain* EntityData = Cast<UEntities_DataAssetMain>(AssetManager->GetPrimaryAssetObject(EntityDataAsset[i])))
			{
				if (EntityDataAsset[i].IsValid())
				{
					AiDataAsset.Add(EntityData->AiData);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("[%s] Ai data not Assigned %s"), *GetClass()->GetName(), *EntityData->Name.ToString());
				}
			}
		}

		if (AiDataAsset.Num() > 0)
		{
			const TArray<FName> Bundles;
			const FStreamableDelegate DataLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnAllDataLoaded);
			AssetManager->LoadPrimaryAssets(AiDataAsset, Bundles, DataLoadedDelegate);
		}
	}
}

void AGameModeMain::OnAllDataLoaded()
{
	// @TODO: Bind this function to different delegate.
	CreateEntities();

	if (AGameStateBaseMain* DefGameState = Cast<AGameStateBaseMain>(GameState))
	{
		if (UGameMode_BaseAsset* GameData = GetGameData())
		{
			DefGameState->SetGameData(GameData);
			DefGameState->RegisterPhaseTaskComplete(LoadGameDataTaskId);
		}
	}
}

FVector AGameModeMain::EntitySpawnStartLocation()
{
	for (TActorIterator<APlayerStartBase> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		APlayerStartBase* PlayerStart = *ActorItr;
		if (PlayerStart)
		{
			return PlayerStart->GetActorLocation();
		}

	}
	return FVector();
}

void AGameModeMain::GenerateEntityLocations(const UGameMode_BaseAsset* GameData)
{
	EntitySpawnLocations.Empty();
	TArray<float> CurrentEntitySpawnSpacing;
	CurrentEntitySpawnSpacing.Init(0.f, 2);
	//const FVector StartEntitySpawnLocation = EntitySpawnStartLocation();

	if (const UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
	{
		for(int EntityIndex = 0; EntityIndex< GameData->Entities.Num(); EntityIndex++)
		{
			if(!GameData->Entities[EntityIndex].IsValid())
			{
				continue;
			}

			if(const UEntities_DataAssetMain* EntityData = Cast<UEntities_DataAssetMain>(AssetManager->GetPrimaryAssetObject(GameData->Entities[EntityIndex])))
			{
				FVector EntityLocation = EntitySpawnStartLocation();
				if(EntityIndex != 0)
				{
				
					const float Itr = EntityIndex % 2;
					const int32 OffsetIndex = FMath::CeilToInt32(Itr);
					if(CurrentEntitySpawnSpacing.IsValidIndex(OffsetIndex))
					{
						CurrentEntitySpawnSpacing[OffsetIndex] += OffsetIndex == 0 ? EntityData->DefaultSpacing: -EntityData->DefaultSpacing;
						EntityLocation.Y += CurrentEntitySpawnSpacing[OffsetIndex];
					}
				}
				if(MainController)
				{
					MainController->GetTerrainPosition(EntityLocation);
				}
				EntitySpawnLocations.Add(EntityLocation);
			}
		}
	}
}

void AGameModeMain::CreateEntities()
{
	const UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
	const USettingsMain* Settings = GetDefault<USettingsMain>();
	
	if (!AssetManager || !Settings || !GetWorld() || !MainController)
	{
		return;
	}
	if(const UGameMode_BaseAsset* CurrentGameData = Cast<UGameMode_BaseAsset>(AssetManager->GetPrimaryAssetObject(CurrentGameDataAssetID)))
	{
		GenerateEntityLocations(CurrentGameData);
		TArray<AActor*> AllEntities;
		for(int i = 0; i < CurrentGameData->Entities.Num(); i++)
		{
			if(!CurrentGameData->Entities[i].IsValid() || !EntitySpawnLocations.IsValidIndex(i))
			{
					continue;
			}
			if (const UEntities_DataAssetMain* EntityData = Cast<UEntities_DataAssetMain>(AssetManager->GetPrimaryAssetObject(CurrentGameData->Entities[i])))
			{
				FTransform SpawnPosition{};
				SpawnPosition.SetLocation(EntitySpawnLocations[i]);
					
				if(const TSoftClassPtr<APawn>* EntityClassPtr = Settings->EntitiesMap.Find(EntityData->EntityType))
				{
					if(EntityClassPtr)
					{
						AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(EntityClassPtr->LoadSynchronous(), SpawnPosition, MainController
									, MainController->GetPawn(), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

						if(NewActor)
						{
							// TODO: Add components and such
							CreateEntityComponent(NewActor, CurrentGameData->Entities[i], i);

							AssignAiController(NewActor, EntityData);

							AllEntities.Add(NewActor);
						}

						UGameplayStatics::FinishSpawningActor(NewActor, SpawnPosition);
						PlayerEntities.Add(NewActor);
					}
				}
			}
		}
	}
}

void AGameModeMain::CreateEntityComponent(AActor* Entity, const FPrimaryAssetId& EntityDataAsset,
	const int32 EntityIndex)
{
	if(UEntities_Component* EntityComponent = NewObject<UEntities_Component>(Entity, TEXT("EntityComponent")))
	{
		Entity->AddInstanceComponent(EntityComponent);
		EntityComponent->RegisterComponent();
		EntityComponent->Initialize(EntityDataAsset, EntityIndex);
	}
	if (UEntities_DecalComponent* DecalComponent = NewObject<UEntities_DecalComponent>(Entity, TEXT("DecalComponent")))
	{
		//DecalComponent->SetupAttachment(Entity->GetRootComponent());
		Entity->AddInstanceComponent(DecalComponent);
		DecalComponent->RegisterComponent();
	}
}

void AGameModeMain::AssignAiController(AActor* Entity, const UEntities_DataAssetMain* EntityDatas)
{
	if (Entity != nullptr && EntityDatas != nullptr && EntityDatas->AiData.IsValid())
	{
		if (const UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
		{
			if (const UAi_DataAssetMain* AiData = Cast<UAi_DataAssetMain>(AssetManager->GetPrimaryAssetObject(EntityDatas->AiData)))
			{
				if (AiData->AIControllerClass.LoadSynchronous())
				{
					FActorSpawnParameters SpawnParams;
					SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
					SpawnParams.Instigator = MainController->GetPawn();
					//SpawnParams.Owner = Entity;

					if (AEntities_AiControllerCommand* AiController = GetWorld()->SpawnActor<AEntities_AiControllerCommand>(
						AiData->AIControllerClass.LoadSynchronous(), FTransform::Identity, SpawnParams))
					{
						if (APawn* EntityPawn = Cast<APawn>(Entity))
						{


							AiController->SetAiData(EntityDatas->AiData);
							AiController->Possess(EntityPawn);
						}
					}
				}
			
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
