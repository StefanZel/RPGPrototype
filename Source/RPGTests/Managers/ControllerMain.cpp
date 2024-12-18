
#include "ControllerMain.h"
#include "RPGTests/Data/AStaticGameData.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "RPGTests/Component/Entities_Component.h"
#include "RPGTests/Component/Entities_DecalComponent.h"
#include "DrawDebugHelpers.h"
#include "Blueprint/WidgetLayoutLibrary.h"


AControllerMain::AControllerMain(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	HitSelectable = nullptr;
	AllySelected = nullptr;
	EnemySelected = nullptr;

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

		if (GetWorld()->LineTraceSingleByChannel(Hit, WorldLocation, WorldLocation + (WorldDirection * 10000.f), TRACE_CHANNEL_TERRAIN))
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
	


	ClearHitSelectable();

	HitSelectable = GetHitSelectable();

	if (HitSelectable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected Actor is %s"), *HitSelectable->GetName());
	}
	HandleSelected(true);
	HandleHighlight(true);
}

AActor* AControllerMain::GetHitSelectable() const
{
	FVector WorldLocation, WorldDirection;
	DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	if (GetWorld())
	{
		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, WorldLocation, WorldLocation + (WorldDirection * 10000.f), TRACE_CHANNEL_ENTITY))
		{
			if (Hit.bBlockingHit && Hit.GetActor() != nullptr)
			{
				DrawDebugLine(GetWorld(), WorldLocation, WorldLocation + (WorldDirection * 10000.f), FColor::Green, false, 1.0f, 0, 1.0f);
				return Hit.GetActor();
			}
		}
	}

	return nullptr;
}

void AControllerMain::HandleHighlight(const bool bHighlight)
{
	if(HitSelectable != nullptr)
	{
		if(UEntities_Component* Entity = UEntities_Component::FindEntityComponent(HitSelectable))
		{
			const FEntitiesSelection AllySelection = Entity->CreateSelection(EEntities_AvailableTypes::Ally);

			Entity->Highlight(bHighlight);
		}
	}
}

void AControllerMain::HandleSelected(const bool bSelect)
{
	if (HitSelectable != nullptr)
	{
		if (UEntities_Component* Entity = UEntities_Component::FindEntityComponent(HitSelectable))
		{
			const FEntitiesSelection AllySelection = Entity->CreateSelection(EEntities_AvailableTypes::Ally);

			Entity->Select(bSelect);
		}
	}
}

void AControllerMain::ClearHitSelectable()
{
	HandleSelected(false);
	HandleHighlight(false);

	/*if (Selection != nullptr)
	{
		Selection = nullptr;
	}*/

	if(HitSelectable != nullptr)
	{
		HitSelectable = nullptr;
	}
}


