
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ControllerMain.h"
#include "RPGTests/Data/Entities/Entities_DataAssetMain.h"
#include "GameModeMain.generated.h"


class UGameMode_BaseAsset;
/**
 * 
 */
UCLASS()
class RPGTESTS_API AGameModeMain : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AGameModeMain(const FObjectInitializer& ObjectInitializer);

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;
	virtual void BeginPlay() override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual void SetPlayerStartLocation(const FVector Location) { PlayerStartLocation = Location; }
	virtual FVector GetPlayerStartLocation() const { return PlayerStartLocation; }

protected:
	void InitGameData();
	void HandleGameDataAssignment();
	void OnGameDataAssigned();
	void OnGameDataLoaded();
	void OnEntityDataLoaded(TArray<FPrimaryAssetId> EntityDataAsset);
	void OnAllDataLoaded();
	FVector EntitySpawnStartLocation();
	void GenerateEntityLocations(const UGameMode_BaseAsset* GameData);
	void CreateEntities();
	void CreateEntityComponent(AActor* Entity, const FPrimaryAssetId& EntityDataAsset, const int32 EntityIndex);
	void AssignAiController(AActor* Entity, const UEntities_DataAssetMain* EntityDatas);

	UGameMode_BaseAsset* GetGameData();

	UPROPERTY()
	FPrimaryAssetId CurrentGameDataAssetID;

	UPROPERTY()
	FVector PlayerStartLocation;


	UPROPERTY()
	TArray<AActor*> PlayerEntities;

	UPROPERTY()
	TArray<FVector> EntitySpawnLocations;

	UPROPERTY()
	AControllerMain* MainController;
	
private:

	UPROPERTY()
	FGuid LoadGameDataTaskId;
	
};
