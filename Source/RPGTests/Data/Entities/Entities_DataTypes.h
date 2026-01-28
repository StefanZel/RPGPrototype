#pragma once

#include "Entities_DataTypes.generated.h"

UENUM()
enum class EEntities_AvailableTypes
{
	None,
	Ally,
	EnemyCommon,
	EnemyRare,
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

UENUM()
enum class EEntities_CommandTypes : uint8
{
	None,
	Navigation,
	Ability,
	AbilityNavigation
};

UENUM()
enum class EEntities_MovementTypes : uint8
{
	None,
	NavigateTo
	// This will containt commands such as sneak, dash, jump, attack, etc.
};

UENUM()
enum class EEntities_AbilityTypes: uint8
{
	None,
	SingleTarget,
	SingleTargetWithMovement,
	AreaOfEffect,
	AreaOfEffectWithMovement
};

UENUM()
enum class EEntities_CommandStatus : uint8
{
	None,
	Active,
	Updating,
	Undo,
	Redo,
	Completed,
	Failed,
	Preview

};

USTRUCT()
struct FEntities_Position
{
	GENERATED_BODY();

public:
	FEntities_Position() : 
		Destination(FVector::ZeroVector),
		Rotation(FRotator::ZeroRotator),
		SourceLocation(FVector::ZeroVector),
		Owner(nullptr)
	{}

	FEntities_Position(
		const FVector& InLocation,
		const FRotator& InRotation,
		const FVector& InSourceLocation
	) :
		Destination(InLocation),
		Rotation(InRotation),
		SourceLocation(InSourceLocation),
		Owner(nullptr)
	{}

	bool IsValid() const { return !Destination.IsNearlyZero(); }

	UPROPERTY()
	FVector Destination;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	FVector SourceLocation;

	UPROPERTY()
	AActor* Owner;



};

USTRUCT()
struct FEntities_BaseCommandData
{
	GENERATED_BODY();
public:
	FEntities_BaseCommandData() :
		CommandType(EEntities_CommandTypes::None),
		MovementType(EEntities_MovementTypes::None),
		HasNavigation(false),
		TargetTransform(FTransform::Identity),
		SourceTransform(FTransform::Identity)
	{}

	FEntities_BaseCommandData(const EEntities_CommandTypes InCmdType, const EEntities_MovementTypes InNvgType, const uint8 HasNavigation) :
		CommandType(InCmdType),
		MovementType(InNvgType),
		HasNavigation(HasNavigation),
		TargetTransform(FTransform::Identity),
		SourceTransform(FTransform::Identity)
	{}

	void SetTargetLocation(const FVector& Location) { TargetTransform.SetLocation(Location); }
	void SetTargetRotation(const FRotator& Rotation) { TargetTransform.SetRotation(Rotation.Quaternion()); }
	void SetSourceLocation(const FVector& Location) { SourceTransform.SetLocation(Location); }
	void SetSourceRotation(const FRotator& Rotation) { SourceTransform.SetRotation(Rotation.Quaternion()); }

	UPROPERTY()
	EEntities_CommandTypes CommandType;

	UPROPERTY()
	EEntities_MovementTypes MovementType;

	UPROPERTY()
	uint8 HasNavigation : 1;

	UPROPERTY()
	FTransform TargetTransform;

	UPROPERTY()
	FTransform SourceTransform;


};

USTRUCT()
struct FEntities_Navigation
{
	GENERATED_BODY();

public:
	FEntities_Navigation(): 
		Location(FVector::ZeroVector),
		Rotation(FRotator::ZeroRotator)
	{}
	FEntities_Navigation(
		const FVector& InLocation,
		const FRotator& InRotation
	):
		Location(InLocation),
		Rotation(InRotation)
	{}

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;



};

USTRUCT(BlueprintType)
struct FEntities_CommandData
{
	GENERATED_BODY();

public:
	FEntities_CommandData(): 
		Id(FGuid::NewGuid()),
		CommandType(EEntities_CommandTypes::None),
		MovementType(EEntities_MovementTypes::None),
		CommandStatus(EEntities_CommandStatus::None),
		TargetTransform(FTransform::Identity),
		SourceTransform(FTransform::Identity),
		SourceActor(nullptr),
		TargetActor(nullptr),
		HasNavigation(false),
		Navigation(FEntities_Navigation())
	{}

	bool IsValid() const { return MovementType != EEntities_MovementTypes::None; }
	void ApplyBaseData(const FEntities_BaseCommandData& BaseData);
	FVector GetLocation() const { return TargetTransform.GetLocation(); }
	FRotator GetRotation() const { return TargetTransform.GetRotation().Rotator(); }

	UPROPERTY()
	FGuid Id;

	UPROPERTY()
	EEntities_CommandTypes CommandType;
	
	UPROPERTY()
	EEntities_MovementTypes MovementType;

	UPROPERTY()
	EEntities_CommandStatus CommandStatus;

	UPROPERTY()
	FTransform TargetTransform;

	UPROPERTY()
	FTransform SourceTransform;

	UPROPERTY()
	AActor* SourceActor;

	UPROPERTY()
	AActor* TargetActor;

	UPROPERTY()
	uint8 HasNavigation : 1;

	UPROPERTY()
	FEntities_Navigation Navigation;
};
