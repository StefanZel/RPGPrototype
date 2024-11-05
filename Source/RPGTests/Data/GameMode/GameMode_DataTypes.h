#pragma once

#include "GameMode_DataTypes.generated.h"

UENUM()
enum class EGamePhaseState
{
	Loading, // Generic Phase
	GenerateNewGameData,
	LoadGameData,
	Initiate,
	Setup,
	PreLoad,
	PreGameplay,
	Gameplay,
	PostGameplay,
	Exit
};

USTRUCT()
struct FPhaseRegistrationData
{
	GENERATED_BODY()

public:
	FPhaseRegistrationData(): GamePhase(EGamePhaseState::Loading), Text(FString()), bIsCompleted(false) {}
	FPhaseRegistrationData(const EGamePhaseState InGamePhase, const FString& InText): GamePhase(InGamePhase), Text(InText), bIsCompleted(false) {}

	UPROPERTY()
	EGamePhaseState GamePhase;

	UPROPERTY()
	FString Text;

	UPROPERTY()
	bool bIsCompleted;
};
