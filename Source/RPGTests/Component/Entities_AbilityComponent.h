// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities_Component.h"
#include "RPGTests/Data/Abilities/Abilities_NormalDataAsset.h"
#include "Entities_AbilityComponent.generated.h"

/**
 * 
 */
UCLASS()
class RPGTESTS_API UEntities_AbilityComponent : public UEntities_Component
{
	GENERATED_BODY()
public:
	UEntities_AbilityComponent(const FObjectInitializer& ObjectInitializer);
	void Initialize(const FPrimaryAssetId& NewEntityDataAsset, const int32 NewEntityIndex) override;

	UFUNCTION()
	static UEntities_AbilityComponent* FindEntityAbilityComponent(const AActor* Entity) { return (Entity ? Entity->FindComponentByClass<UEntities_AbilityComponent>() : nullptr); }

	void ActivateAbility(const FPrimaryAssetId& Ability);
	void UpdateAbilityPosition(const FVector& Position);
	void ExecuteAbility();
	FPrimaryAssetId GetAbilityBySlot(int32 AbilitySlot);
private:
	void SetAbilityDataAssets();
	bool IsAbilityValid() const;

	UAbilities_NormalDataAsset* GetAbilityData() const;
	float GetAbilityRange();
	UPROPERTY()
	TArray<FPrimaryAssetId> AvailableAbilityDataAssets;

	UPROPERTY()
	FPrimaryAssetId SelectedAbility;

	UPROPERTY()
	AActor* ActiveAbility;
};
