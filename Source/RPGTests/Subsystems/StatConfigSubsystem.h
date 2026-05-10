// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGTests/Data/Entities/StatConfigDataAsset.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StatConfigSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class RPGTESTS_API UStatConfigSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	const UStatConfigDataAsset* GetStatConfig() const { return StatConfig; }
	
	const FStatConfig* GetConfig(const FGameplayTag& StatTag) const
	{
		return StatConfig ? StatConfig->GetConfig(StatTag) : nullptr;
	}
	
private:
	UPROPERTY()
	TObjectPtr<UStatConfigDataAsset> StatConfig;
	
};
