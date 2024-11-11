
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RPGTests/Selectable.h"
#include "RPGTests/Data/Entities/Entities_DataTypes.h"
#include "ControllerMain.generated.h"

class APlayerMain;
struct FEntitiesSelection;

DECLARE_MULTICAST_DELEGATE(FOnMapDataLoadedDelegate);

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

	FOnMapDataLoadedDelegate OnMapDataLoaded;
	FPrimaryAssetId GetMapData() const { return MapData; }

protected:
	
	UPROPERTY()
	FEntitiesSelection Selection;

	
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
};
