// Fill out your copyright notice in the Description page of Project Settings.


#include "StatConfigSubsystem.h"

#include "RPGTests/Data/Entities/StatConfigDataAsset.h"

void UStatConfigSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	const FSoftObjectPath ConfigPath(TEXT("/Game/Data/Config/DA_StatConfig.Da_StatConfig"));
	
	StatConfig = Cast<UStatConfigDataAsset>(ConfigPath.TryLoad());
	
	ensureMsgf(StatConfig, TEXT("UStatConfigSubsystem: Failed to load StatConfigDataAsset. Check if DA_StatConfig exists at the expected path."));
}
