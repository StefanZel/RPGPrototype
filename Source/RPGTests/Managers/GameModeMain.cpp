
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
#include "RPGTests/Props/EnemySpawnPoint.h"
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
		// TODO: Generate map data and create new struct for it.
		MainGameState->CallOrRegisterPhase_OnGamePhaseLoadGameData(FOnGamePhaseLoadGameDataDelegate::FDelegate::CreateUObject(this, &ThisClass::InitGameData),
			LoadGameDataTaskId, 
			FString("GameMode - LoadGameData"));
		MainGameState->CallOrRegisterPhase_OnGamePhaseInitiate(FOnGamePhaseInitiateDelegate::FDelegate::CreateUObject(this, &ThisClass::CreateEntities),
			CreateEntitiesTaskId,
			FString("GameMode - CreateEntities"));
		MainGameState->CallOrRegisterPhase_OnGamePhaseInitiate(FOnGamePhaseInitiateDelegate::FDelegate::CreateUObject(this, &ThisClass::CreateEnemies),
			CreateEnemyTaskId,
			FString("GameMode - CreateEnemies"));

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
				if (UMap_BaseDataAsset* MapData = Cast<UMap_BaseDataAsset>(CurrentGameData->MapData))
				{
					EntityDataAsset.Append(MapData->AvailableTypes);
				}
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
			const FStreamableDelegate DataLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnAiDataLoaded, EntityDataAsset);
			AssetManager->LoadPrimaryAssets(AiDataAsset, Bundles, DataLoadedDelegate);
		}
	}
}

void AGameModeMain::OnAiDataLoaded(TArray<FPrimaryAssetId> EntityDataAsset)
{
	if (UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
	{
		TArray<FPrimaryAssetId> AbilityDataAsset;
		for (int i = 0; i < EntityDataAsset.Num(); i++)
		{
			if (const UEntities_DataAssetMain* EntityData = Cast<UEntities_DataAssetMain>(AssetManager->GetPrimaryAssetObject(EntityDataAsset[i])))
			{
				if (EntityDataAsset[i].IsValid())
				{
					for (const FPrimaryAssetId& Ability : EntityData->Abilities)
					{
						AbilityDataAsset.AddUnique(Ability);
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("[%s] Ability data not Assigned %s"), *GetClass()->GetName(), *EntityData->Name.ToString());
				}
			}
		}
		if (AbilityDataAsset.Num() > 0)
		{
			const TArray<FName> Bundles;
			const FStreamableDelegate DataLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnAllDataLoaded);
			AssetManager->LoadPrimaryAssets(AbilityDataAsset, Bundles, DataLoadedDelegate);
		}
	}
}

void AGameModeMain::OnAllDataLoaded()
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

void AGameModeMain::GenerateEnemySpawnLocation()
{
	EnemySpawnLocations.Empty();

	for (TActorIterator<AEnemySpawnPoint> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AEnemySpawnPoint* ESpawnPoint = *ActorItr;
		if (ESpawnPoint)
		{
			EnemySpawnLocations.Add(ESpawnPoint->GetActorTransform());
		}
	}
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
	
	if (!AssetManager || !GetWorld() || !MainController)
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
					
				if(EntityData->BPClass)
				{
					AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(EntityData->BPClass.LoadSynchronous(), SpawnPosition, MainController
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
		OnEntityCreationComplete();
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

void AGameModeMain::OnEntityCreationComplete()
{
	if (AGameStateBaseMain* DefGameState = Cast<AGameStateBaseMain>(GameState))
	{
			DefGameState->RegisterPhaseTaskComplete(CreateEntitiesTaskId);
	}
}

void AGameModeMain::CreateEnemies()
{
	const UAssetManager* AssetManager = UAssetManager::GetIfInitialized();

	if (!AssetManager || !GetWorld() || !MainController)
	{
		return;
	}
	if (const UGameMode_BaseAsset* CurrentGameData = Cast<UGameMode_BaseAsset>(AssetManager->GetPrimaryAssetObject(CurrentGameDataAssetID)))
	{
		if (UMap_BaseDataAsset* MapData = Cast<UMap_BaseDataAsset>(CurrentGameData->MapData))
		{
			MapData->SetEntitiesWeight();
			MapData->SetEnemiesToSpawn();

			//SetEnemiesToSpawn(MapData);
			GenerateEnemySpawnLocation();
			
			UE_LOG(LogTemp, Log, TEXT("Entities to spawn: %d"), MapData->EnemiesToSpawn.Num());

			for (int i = 0; i < MapData->EnemiesToSpawn.Num(); i++)
			{
				if (!EnemySpawnLocations.IsValidIndex(i))
				{
					continue;
				}

				UE_LOG(LogTemp, Log, TEXT("Spawning entity: %s at location: %s"), *MapData->EnemiesToSpawn[i]->GetName(), *EnemySpawnLocations[i].ToString());

				FTransform SpawnPosition{};
				SpawnPosition.SetLocation(EnemySpawnLocations[i].GetLocation());
				SpawnPosition.SetRotation(EnemySpawnLocations[i].GetRotation());

				if (MapData->EnemiesToSpawn[i]->BPClass)
				{
					AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(MapData->EnemiesToSpawn[i]->BPClass.LoadSynchronous(), SpawnPosition, MainController
						, MainController->GetPawn(), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

					if (NewActor)
					{
						FPrimaryAssetId EnemyAssetID = MapData->EnemiesToSpawn[i]->GetPrimaryAssetId();
						CreateEntityComponent(NewActor, EnemyAssetID, i);

						AssignAiController(NewActor, MapData->EnemiesToSpawn[i]);
					}

					UGameplayStatics::FinishSpawningActor(NewActor, SpawnPosition);
				}
			}

			OnEnemiesCreated();

		}
	}

}

void AGameModeMain::OnEnemiesCreated()
{
	if (AGameStateBaseMain* DefGameState = Cast<AGameStateBaseMain>(GameState))
	{
			DefGameState->RegisterPhaseTaskComplete(CreateEnemyTaskId);
	}
}


void AGameModeMain::SetMapdata()
{

}

UMap_BaseDataAsset* AGameModeMain::CreateMapData()
{
	UMap_BaseDataAsset* NewMapData = NewObject<UMap_BaseDataAsset>();
	NewMapData->MapLevel = 1;
	NewMapData->SetEntitiesWeight();

	return NewMapData;
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
