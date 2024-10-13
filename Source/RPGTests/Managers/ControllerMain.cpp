
#include "ControllerMain.h"
#include "RPGTests/Data/AStaticGameData.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"



AControllerMain::AControllerMain(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	HitSelectable = nullptr;
}

void AControllerMain::BeginPlay()
{
	Super::BeginPlay();

}

void AControllerMain::GetTerrainPosition(FVector& TerrainPosition) const
{
	FHitResult Hit;
	FCollisionQueryParams CollisionParameters;
	FVector TraceStart = TerrainPosition;
	TraceStart.Z += 10000.f;
	FVector TraceEnd = TerrainPosition;
	TraceEnd.Z -= 10000.f;

	if (GetWorld())
	{
		if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, TRACE_CHANNEL_TERRAIN, CollisionParameters))
		{
			TerrainPosition = Hit.ImpactPoint;
		}
	}
}

void AControllerMain::GetMousePositionOnTerrain(FVector& TerrainPosition) const
{
	FVector WorldLocation, WorldDirection;
	DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	if (GetWorld())
	{
		FHitResult Hit;

		if (GetWorld()->LineTraceSingleByChannel(Hit, WorldLocation, WorldLocation + (WorldDirection + 10000.f), TRACE_CHANNEL_TERRAIN))
		{
			if (Hit.bBlockingHit)
			{
				TerrainPosition = Hit.Location;
			}
		}
	}
}

void AControllerMain::Select()
{
	// TODO Ally, Enemy, and Resource selection
	ClearHitSelectable();

	HitSelectable = GetHitSelectable();
}

AActor* AControllerMain::GetHitSelectable() const
{
	FVector WorldLocation, WorldDirection;
	DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	if (GetWorld())
	{
		FHitResult Hit;

		if (GetWorld()->LineTraceSingleByChannel(Hit, WorldLocation, WorldLocation + (WorldDirection + 10000.f), TRACE_CHANNEL_ENTITY))
		{
			if (Hit.bBlockingHit && Hit.GetActor() != nullptr)
			{
				return Hit.GetActor();
			}
		}
	}

	return nullptr;
}

void AControllerMain::ClearHitSelectable()
{
		if(HitSelectable != nullptr)
		{
			HitSelectable = nullptr;
		}
}


