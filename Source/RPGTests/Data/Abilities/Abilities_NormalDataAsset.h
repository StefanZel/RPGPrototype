// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities_DataTypes.h"
#include "RPGTests/Data/Entities/Entities_DataTypes.h"
#include "Engine/DataAsset.h"
#include "NiagaraSystem.h"
#include "Abilities_NormalDataAsset.generated.h"


/**
 * 
 */
UCLASS()
class RPGTESTS_API UAbilities_NormalDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Asset Settings")
	FPrimaryAssetType DataType;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(DataType, GetFName());
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Specs")
	TSoftClassPtr<AActor> AbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Specs")
	TSoftObjectPtr<UNiagaraSystem> NiagaraEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Specs")
	TSoftObjectPtr<UStaticMesh> MeshComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Specs", Meta = (Categories = "Ability.Type"))
	FGameplayTag Type;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Specs", Meta = (Categories = "Ability.Use"))
	FGameplayTag Use;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Specs")
	EEntities_MovementTypes AbilityMovement;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Specs")
	FAbilities_DeployParams DeployParams;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Specs")
	FAbilities_Cost AbilityCost;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Specs")
	FAbilities_Damage AbilityDamage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Specs", Meta = (Categories = "Ability, Damage"))
	FGameplayTagContainer TriggeredBy;
	
};


