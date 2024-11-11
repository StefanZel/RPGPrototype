// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "SettingsMain.generated.h"

enum class EEntities_AvailableTypes;
/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Main Game Settings"))
class RPGTESTS_API USettingsMain : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()
public:
	USettingsMain();

	UPROPERTY(Config, EditAnywhere, Category=Entities)
	TMap<EEntities_AvailableTypes, TSoftClassPtr<APawn>> EntitiesMap;
};
