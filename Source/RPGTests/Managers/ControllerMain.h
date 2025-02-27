
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RPGTests/Selectable.h"
#include "RPGTests/Data/Entities/Entities_DataTypes.h"
#include "ControllerMain.generated.h"

class APlayerMain;
class UCommandData;
class UCommandBase;

DECLARE_MULTICAST_DELEGATE(FOnMapDataLoadedDelegate);

UENUM()
enum class ECommandState : uint8
{
	None,
	Initiated,
	Held
};

UCLASS()
class RPGTESTS_API AControllerMain : public APlayerController, public ISelectable
{
	GENERATED_BODY()

public:

	AControllerMain(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	
	void GetTerrainPosition(FVector& TerrainPosition) const;
	void GetMousePositionOnTerrain(FVector& TerrainPosition) const;

	virtual void Select() override;
	virtual void Command() override;
	virtual void CommandHold() override;
	virtual void CommandEnd() override;

	FOnMapDataLoadedDelegate OnMapDataLoaded;
	FPrimaryAssetId GetMapData() const { return MapData; }

protected:


	
	void AssignCommandTargetLocation();
	FEntities_BaseCommandData CreateBaseCommandData();
	void GetCommandType(EEntities_CommandTypes& CommandType, uint8& HasNavigation) const;
	void GetSourceLocation(FVector& SourceLocation);
	UCommandBase* CreateCommand(const FEntities_BaseCommandData& BaseCommandData);
	void UpdateCommandData(const FEntities_BaseCommandData& BaseCommandData, FEntities_CommandData& CommandData, const bool Preview = false);
	void GetCommandNavigationData(FEntities_CommandData& CommandData) const;

	UPROPERTY()
	ECommandState CommandState;

	UPROPERTY()
	FVector CommandTargetLocation;

	void CommandHistory(const FGuid Id, const bool Success);


	AActor* GetHitSelectable() const;
	void HandleHighlight(const bool bHighlight);
	void HandleSelected(const bool bSelect);
	void ClearHitSelectable();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Map Data", meta = (AllowedTypes = "MapData"))
    	FPrimaryAssetId MapData;

private:

	UPROPERTY()
	FVector SelectStartWorldLocation;


	UPROPERTY()
	FVector2D SelectStartViewportLocation;


	UPROPERTY()
	FVector2D SelectStartScreenLocation;


	UPROPERTY()
	AActor* HitSelectable;

	UPROPERTY()
	AActor* AllySelected;

	UPROPERTY()
	AActor* EnemySelected;
};
