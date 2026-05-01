// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "UObject/ObjectMacros.h"
#include "RPGTests/Data/Entities/Entities_Tags.h"
#include "Abilities_DataTypes.generated.h"


USTRUCT(BlueprintType)
struct FAbilities_DeployParams
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Range;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Reach;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Size;
	
};

USTRUCT(BlueprintType)
struct FAbilities_Cost
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Cost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Stat.Resource"))
	FGameplayTag CostType;
};


USTRUCT(BlueprintType)
struct FAbilities_Damage
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ScaleFactor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Damage"))
	FGameplayTag DamageType;
	
};
