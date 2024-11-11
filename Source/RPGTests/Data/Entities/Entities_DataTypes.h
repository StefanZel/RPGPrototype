#pragma once

#include "Entities_DataTypes.generated.h"

UENUM()
enum class EEntities_AvailableTypes
{
	None,
	Ally,
	Enemy,
	Resource
};

UENUM()
enum class EEntities_Sizes
{
	None,
	NormalSmall,
	NormalAverage,
	NormalLarge,
	FourLegsSmall,
	FourLegsLarge
};

USTRUCT()
struct FEntitiesSelection
{
	GENERATED_BODY();

public:
	FEntitiesSelection() : Ally(nullptr) {}
	FEntitiesSelection(AActor* AllySelection) : Ally(AllySelection) {}

	UPROPERTY()
	AActor* Ally;

};
