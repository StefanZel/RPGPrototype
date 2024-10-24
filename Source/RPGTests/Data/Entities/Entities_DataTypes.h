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
