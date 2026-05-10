// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGTests/Data/Battle/Battle_DataTypes.h"
#include "UObject/Object.h"
#include "DamageCalculator.generated.h"

/**
 * 
 */
UCLASS()
class RPGTESTS_API UDamageCalculator : public UObject
{
	GENERATED_BODY()
public:
	void Reset();
	
	void CalculateAndQueue(const UCommandBase* Command);
	FPendingDamage* FindPendingForTarget(const AActor* Target);
	void CommitAll();
	const TArray<FPendingDamage>& GetPendingDamage() const { return PendingDamages; }
private:
	float GetBaseDamage(const AActor* Attacker) const;
	float ApplyMitigation(const float RawDamage, const AActor* Defender) const;
	
	UPROPERTY()
	TArray<FPendingDamage> PendingDamages;
};

