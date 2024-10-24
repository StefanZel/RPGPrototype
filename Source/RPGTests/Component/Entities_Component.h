
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGTests/Data/Entities/Entities_DataAssetMain.h"
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

	UFUNCTION()
	static UEntities_Component* FindEntityComponent(const AActor* Entity) { return (Entity ? Entity->FindComponentByClass<UEntities_Component>() : nullptr); }
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Player Input Settings")
	UDataAsset* EntityDataAsset;

	UPROPERTY()
	UEntities_DataAssetMain* EntityData;

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
