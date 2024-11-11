
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGTests/Data/Entities/Entities_DataAssetMain.h"
#include "RPGTests/Data/Entities/Entities_DataTypes.h"
#include "Materials/MaterialInterface.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Entities_Component.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSelectedChangeDelegate, const bool);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RPGTESTS_API UEntities_Component : public UActorComponent
{
	GENERATED_BODY()

public:
	UEntities_Component(const FObjectInitializer& ObjectInitializer);

	void Initialize(const FPrimaryAssetId& NewEntityDataAsset, const int32 NewEntityIndex);
	UFUNCTION()
	static UEntities_Component* FindEntityComponent(const AActor* Entity) { return (Entity ? Entity->FindComponentByClass<UEntities_Component>() : nullptr); }
	virtual FEntitiesSelection CreateSelection(const EEntities_AvailableTypes SelectionType);
	
	UPROPERTY()
	int32 EntityIndex ;

	FOnSelectedChangeDelegate OnSelectedChange;
	virtual void Highlight(const bool bHighlight = true);
	virtual void Select(const bool bSelect = true);
	virtual UMaterialInstance* GetHighlightMaterial();
	virtual UMaterialInstance* GetSelectMaterial();


	virtual UEntities_DataAssetMain* GetData() const;
	virtual EEntities_AvailableTypes GetTypes() const;
	virtual EEntities_Sizes GetSizes() const;
	virtual FVector GetSizeColl();


protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	FPrimaryAssetId EntityDataAssetID;


};
