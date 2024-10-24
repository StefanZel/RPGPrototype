// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/DecalComponent.h"
#include "Entities_DecalComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RPGTESTS_API UEntities_DecalComponent : public UDecalComponent
{
	GENERATED_BODY()

public:
	UEntities_DecalComponent(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	void UpdateSelected(const bool bSelected);
};
