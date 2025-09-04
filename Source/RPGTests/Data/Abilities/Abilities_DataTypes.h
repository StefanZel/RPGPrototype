// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "UObject/ObjectMacros.h"
#include "Abilities_DataTypes.generated.h"

UENUM()
enum class EAbilities_Type
{
	None,
	Melee,
	Ranged
};

UENUM()
enum class EAbilities_Shapes 
{ 
	None, 
	SingleTarget, 
	RoundAOE, 
	RectangleAOE 
};

UENUM()
enum class EAbilities_Uses 
{ 
	None, 
	Damage, 
	Buff, 
	Heal, 
	Support, 
	Trap, 
	Block 
};

UENUM()
enum class EAbilities_Elements 
{ 
	None, 
	Water, 
	Fire, 
	Eearth, 
	Wind 
};
