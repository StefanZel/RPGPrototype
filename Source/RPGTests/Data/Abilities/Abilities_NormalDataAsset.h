// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities_DataTypes.h"
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Specs")
	EAbilities_Type Type;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Specs")
	EAbilities_Shapes Shape;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Specs")
	EAbilities_Uses Use;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Specs")
	EAbilities_Elements Element;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Specs")
	float XRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Specs")
	float YRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Stats")
	int BaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Stats")
	float DamageScaleFactor;
};


