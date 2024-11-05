

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
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

protected:
	void InitGameData();
	void HandleGameDataAssignment();
	void OnGameDataAssigned();
	void OnGameDataLoaded();

	UGameMode_BaseAsset* GetGameData();

	UPROPERTY()
	FPrimaryAssetId CurrentGameDataAssetID;
	
private:

	UPROPERTY()
	FGuid LoadGameDataTaskId;
	
};
