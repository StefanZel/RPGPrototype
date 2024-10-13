
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RPGTests/Selectable.h"
#include "ControllerMain.generated.h"

class APlayerMain;
struct FGameEntities_AllySelection;

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


protected:
	
	AActor* GetHitSelectable() const;
	void ClearHitSelectable();

private:
	
	UPROPERTY()
	AActor* HitSelectable;
};
