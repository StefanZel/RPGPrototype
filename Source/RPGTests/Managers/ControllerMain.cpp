
#include "ControllerMain.h"
#include "RPGTests/Data/AStaticGameData.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "RPGTests/Component/Entities_Component.h"
#include "RPGTests/Component/Entities_AbilityComponent.h"
#include "RPGTests/Component/Entities_DecalComponent.h"
#include "DrawDebugHelpers.h"
#include "Blueprint/WidgetLayoutLibrary.h"


AControllerMain::AControllerMain(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	HitSelectable = nullptr;
	AllySelected = nullptr;
	EnemySelected = nullptr;

	CommandState = ECommandState::None;
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
}

void AControllerMain::Command()
{
	CommandState = ECommandState::Initiated;

	AssignCommandTargetLocation();
}

void AControllerMain::CommandHold()
{
	CommandState = ECommandState::Held;
}

void AControllerMain::CommandEnd()
{
	const FEntities_BaseCommandData BaseCommandData = CreateBaseCommandData();

	if (UCommandBase* NewCommand = CreateCommand(BaseCommandData))
	{
		NewCommand->Execute();
	}

	CommandState = ECommandState::None;

}

void AControllerMain::SelectAbility(int32 AbilitySlot)
{
	if (HitSelectable)
	{
		UEntities_AbilityComponent* AbilityComponent = UEntities_AbilityComponent::FindEntityAbilityComponent(HitSelectable);

		AbilitySelected = AbilityComponent->GetAbilityBySlot(AbilitySlot);
	}
}

void AControllerMain::AssignCommandTargetLocation()
{
	FVector MouseLocation = FVector::ZeroVector;
	GetMousePositionOnTerrain(MouseLocation);
	CommandTargetLocation = MouseLocation;
}

FEntities_BaseCommandData AControllerMain::CreateBaseCommandData()
{
	EEntities_CommandTypes CommandType = EEntities_CommandTypes::None;
	uint8 HasNavigation = false;

	GetCommandType(CommandType, HasNavigation);

	FVector MouseLocation = FVector::ZeroVector;
	GetMousePositionOnTerrain(MouseLocation);

	FVector SourceLocation = FVector::ZeroVector;
	GetSourceLocation(SourceLocation);



	const FVector TargetLocation = CommandState > ECommandState::None ? CommandTargetLocation : MouseLocation;

	const FRotator TargetRotation = (MouseLocation - TargetLocation).Length() > 150.f
		? FRotationMatrix::MakeFromX(MouseLocation - TargetLocation).Rotator()
		: FRotationMatrix::MakeFromX(TargetLocation - SourceLocation).Rotator();

	FEntities_BaseCommandData BaseCommandData = FEntities_BaseCommandData(CommandType, HasNavigation);
	BaseCommandData.SetTargetLocation(TargetLocation);
	BaseCommandData.SetTargetRotation(TargetRotation);
	BaseCommandData.SetSourceLocation(SourceLocation);

	return BaseCommandData;
}

void AControllerMain::GetCommandType(EEntities_CommandTypes& CommandType, uint8& HasNavigation) const
{
	// This will change later. It will depend on mouse position, spell selection, etc.

	CommandType = EEntities_CommandTypes::NavigateTo;
	HasNavigation = true;
}

void AControllerMain::GetSourceLocation(FVector& SourceLocation)
{
	if (HitSelectable != nullptr)
	{
		SourceLocation = HitSelectable->GetActorLocation();
	}
}

UCommandBase* AControllerMain::CreateCommand(const FEntities_BaseCommandData& BaseCommandData)
{
	if (HitSelectable != nullptr)
	{
		if (UEntities_Component* Entity = UEntities_Component::FindEntityComponent(HitSelectable))
		{
			if (UEntities_DataAssetMain* EntityData = Entity->GetData())
			{
				if (const TSoftClassPtr<UCommandBase>* CommandClassPtr = EntityData->Commands.Find(BaseCommandData.CommandType))
				{
					if (UCommandBase* NewCommand = NewObject<UCommandBase>(this, CommandClassPtr->LoadSynchronous()))
					{

						FEntities_CommandData CommandData = FEntities_CommandData();
						UpdateCommandData(BaseCommandData, CommandData);
						NewCommand->Data = CommandData;
						NewCommand->SetSelected(HitSelectable);

						return NewCommand;
					}
				}
			}
		}
	}

	return nullptr;
}

void AControllerMain::UpdateCommandData(const FEntities_BaseCommandData& BaseCommandData, FEntities_CommandData& CommandData, const bool Preview)
{
	Preview ? CommandData.Id = FGuid() : CommandData.Id = FGuid::NewGuid();

	CommandData.ApplyBaseData(BaseCommandData);

	//CommandData.SourceActor = HitSelectable;

	// TODO: Change this after refactoring actor spawning. Don't forget to change GetHitSelectable() to return actor on specific location
	if (EnemySelected != nullptr)
	{
		CommandData.TargetActor = EnemySelected;
	}

	if (CommandData.HasNavigation)
	{
		GetCommandNavigationData(CommandData);
	}
}

void AControllerMain::GetCommandNavigationData(FEntities_CommandData& CommandData) const
{
	CommandData.Navigation = FEntities_Navigation();
}

void AControllerMain::CommandHistory(const FGuid Id, const bool Success)
{
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
			Entity->Select(bSelect);
		}
	}
}

void AControllerMain::ClearHitSelectable()
{
	HandleSelected(false);

	if (AbilitySelected.IsValid())
	{
		AbilitySelected = FPrimaryAssetId();
	}

	if(HitSelectable != nullptr)
	{
		HitSelectable = nullptr;
	}
}


