// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomAssetManager.h"

UCustomAssetManager& UCustomAssetManager::Get()
{
	UAssetManager* Singleton = UAssetManager::GetIfInitialized();
	
	if (ensureMsgf(Singleton, TEXT("AssetManager not initialized yet")))
	{
		return *CastChecked<UCustomAssetManager>(Singleton);
	}
	
	return *NewObject<UCustomAssetManager>();
}

void UCustomAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
}
