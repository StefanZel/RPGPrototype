// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "CustomAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class RPGTESTS_API UCustomAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	static UCustomAssetManager& Get();
	virtual void StartInitialLoading() override;
	
	
	template<typename T>
	T* GetAsset(const FPrimaryAssetId& AssetId) const
	{
		static_assert(TIsDerivedFrom<T, UPrimaryDataAsset>::IsDerived, "T Must derive from UPrimaryDataAsset");
		
		if (!AssetId.IsValid()) return nullptr;
		
		return Cast<T>(GetPrimaryAssetObject(AssetId));
	}
	
	template<typename T>
	void LoadAsset(const FPrimaryAssetId& AssetId, TFunction<void(T*)> OnLoaded, const TArray<FName>& Bundles = {})
	{
		if (!AssetId.IsValid())
		{
			OnLoaded(nullptr);
			return;
		}
		
		FStreamableDelegate Delegate = FStreamableDelegate::CreateLambda([this, AssetId, OnLoaded]()
		{
			OnLoaded(GetAsset<T>(AssetId));	
		});
		LoadPrimaryAsset(AssetId, Bundles, Delegate);
	}
	
	template<typename T>
	void LoadAssets(const TArray<FPrimaryAssetId>& AssetIds, TFunction<void(TArray<T*>)> OnLoaded, const TArray<FName>& Bundles = {})
	{
		if (AssetIds.IsEmpty())
		{
			OnLoaded({});
			return;
		}
		FStreamableDelegate Delegate = FStreamableDelegate::CreateLambda([this, AssetIds, OnLoaded]()
		{
			TArray<T*> Results;
			for (const FPrimaryAssetId& Id : AssetIds)
			{
				if (T* Asset = GetAsset<T>(Id))
				{
					Results.Add(Asset);
				}
			}
			OnLoaded(Results);
		});
		LoadPrimaryAssets(AssetIds, Bundles, Delegate);
	}
};
