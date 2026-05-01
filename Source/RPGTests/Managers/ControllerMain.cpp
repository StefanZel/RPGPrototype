
#include "ControllerMain.h"
#include "RPGTests/Data/AStaticGameData.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "RPGTests/Component/Entities_Component.h"
#include "RPGTests/Component/Entities_AbilityComponent.h"
#include "RPGTests/Component/Entities_DecalComponent.h"
#include "DrawDebugHelpers.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Engine/AssetManager.h"
#include "RPGTests/Component/Entities_StatComponent.h"


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
	if (AbilitySelected.IsValid() && HitSelectable)
	{
		ExecuteAbility();
	}



	ClearHitSelectable();

	HitSelectable = GetHitSelectable();

	if (HitSelectable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected Actor is %s"), *HitSelectable->GetName());
		
		if (UEntities_StatComponent* StatComponent = UEntities_StatComponent::FindEntityStatComponent(HitSelectable))
		{
			StatComponent->DebugDumpStats();
		}
	}
	HandleSelected(true);
}

void AControllerMain::CommandMove()
{
	CommandState = ECommandState::Initiated;

	AssignCommandTargetLocation();
}

void AControllerMain::CommandMoveHold()
{
	CommandState = ECommandState::Held;
}

void AControllerMain::CommandMoveEnd()
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

		AbilityComponent->ActivateAbility(AbilitySelected);

		StartUpdatingAbility();
	}
}

void AControllerMain::ToggleCharacterSheet()
{
	if (!HitSelectable) return;
	
	if (!CharacterSheetWidget)
	{
		CharacterSheetWidget = CreateWidget<UCharacterSheetWidget>(this, CharacterSheetClass);
	}
	
	if (CharacterSheetWidget->IsInViewport())
	{
		CharacterSheetWidget->RemoveFromParent();
	}
	else
	{
		CharacterSheetWidget->InitForActor(HitSelectable);
		CharacterSheetWidget->AddToViewport();
	}
}

void AControllerMain::AssignCommandTargetLocation()
{
	FVector MouseLocation = FVector::ZeroVector;
	GetMousePositionOnTerrain(MouseLocation);
	MovementLocation = MouseLocation;
}

FEntities_BaseCommandData AControllerMain::CreateBaseCommandData()
{
	EEntities_CommandTypes CommandTypes = EEntities_CommandTypes::None;
	EEntities_MovementTypes MovementTypes = EEntities_MovementTypes::None;

	GetCommandType(CommandTypes, MovementTypes);

	FVector SourceLocation = FVector::ZeroVector;
	GetSourceLocation(SourceLocation);
	
	FVector MoveLocation = SourceLocation;
	
	switch (CommandTypes)
	{
		case EEntities_CommandTypes::Navigation:
			GetMousePositionOnTerrain(MoveLocation);
			break;
		case EEntities_CommandTypes::Ability:
			MoveLocation = SourceLocation;
			break;
		case EEntities_CommandTypes::AbilityNavigation:
			GetAbilityMovementPosition(MoveLocation);
			break;
			
		default: break;
	}
	
	FVector FacePoint = (CommandTypes == EEntities_CommandTypes::Navigation) ? MoveLocation : AbilityPosition;
	FVector LookDirection = FacePoint - SourceLocation;
	
	FVector LookAtTarget = (CommandTypes == EEntities_CommandTypes::AbilityNavigation || 
							CommandTypes == EEntities_CommandTypes::Ability) 
							? AbilityPosition
							: MoveLocation;
	
	FRotator TargetRotation;
	if (!LookDirection.IsNearlyZero())
	{
		TargetRotation = LookDirection.Rotation();
	}
	else
	{
		TargetRotation = HitSelectable ? HitSelectable->GetActorRotation() : FRotator::ZeroRotator;
	}

	TargetRotation.Pitch = 0.f;
	TargetRotation.Roll = 0.f;
	
	/*const FVector TargetLocation = CommandState > ECommandState::None ? MovementLocation : MoveLocation;
	const FRotator TargetRotation = (MoveLocation - LookAtTarget).Length() > 150.f
		? FRotationMatrix::MakeFromX(MoveLocation - LookAtTarget).Rotator()
		: FRotationMatrix::MakeFromX(LookAtTarget - SourceLocation).Rotator();*/

	FEntities_BaseCommandData BaseCommandData = FEntities_BaseCommandData(CommandTypes, MovementTypes);
	BaseCommandData.SetTargetLocation(MoveLocation);
	BaseCommandData.SetTargetRotation(TargetRotation);
	BaseCommandData.SetSourceLocation(SourceLocation);

	return BaseCommandData;
}

void AControllerMain::GetCommandType(EEntities_CommandTypes& CommandTypes, EEntities_MovementTypes& MovementTypes) const
{
	CommandTypes = EEntities_CommandTypes::None;
	MovementTypes = EEntities_MovementTypes::None;
	
	if (!HitSelectable) return;
	
	if (AbilitySelected.IsValid())
	{
		if (const UAbilities_NormalDataAsset* AbilityData = GetAbilityDataAsset())
		{
			const bool bIsStatic = AbilityData->AbilityMovement == EEntities_MovementTypes::None;
			
			CommandTypes = bIsStatic ? EEntities_CommandTypes::Ability : EEntities_CommandTypes::AbilityNavigation;
			MovementTypes = AbilityData->AbilityMovement;
			if (IsSourceInAbilityReach())
			{
				CommandTypes = EEntities_CommandTypes::Ability;
				MovementTypes = EEntities_MovementTypes::RotateTo;
			}
			return;
		}
	}
	CommandTypes = EEntities_CommandTypes::Navigation;
	MovementTypes = EEntities_MovementTypes::NavigateTo;
}

void AControllerMain::GetSourceLocation(FVector& SourceLocation) const
{
	if (HitSelectable != nullptr)
	{
		SourceLocation = HitSelectable->GetActorLocation();
	}
}

bool AControllerMain::IsSourceInAbilityReach() const
{
	const UAbilities_NormalDataAsset* AbilityData = GetAbilityDataAsset();
	if (!AbilityData) return false;
	
	FVector SourceLocation;
	GetSourceLocation(SourceLocation);
	
	float Distance = FVector::DistSquared(SourceLocation, AbilityPosition);
	float Reach  = FMath::Square(AbilityData->DeployParams.Reach);
	
	return Distance <= Reach;
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

	if (CommandData.HasNavigation())
	{
		GetCommandNavigationData(CommandData);
	}
}

void AControllerMain::GetCommandNavigationData(FEntities_CommandData& CommandData) const
{
	CommandData.Navigation = FEntities_Navigation();
}

void AControllerMain::StartUpdatingAbility()
{	
	if (!GetWorld()->GetTimerManager().IsTimerActive(HandleAbilityUpdate))
	{
		GetWorld()->GetTimerManager().SetTimer(HandleAbilityUpdate, this, &ThisClass::UpdateAbility, 0.01f, true);
	}
}

void AControllerMain::UpdateAbility()
{
	if (AbilitySelected.IsValid() && HitSelectable)
	{
		UEntities_AbilityComponent* AbilityComponent = UEntities_AbilityComponent::FindEntityAbilityComponent(HitSelectable);

		GetMousePositionOnTerrain(AbilityPosition);
		
		AbilityComponent->UpdateAbilityPosition(AbilityPosition);
	}
}

void AControllerMain::EndUpdatingAbility()
{
	GetWorld()->GetTimerManager().ClearTimer(HandleAbilityUpdate);
}

void AControllerMain::GetAbilityMovementPosition(FVector& TerrainPosition)
{
	if (const UAbilities_NormalDataAsset* AbilityData = GetAbilityDataAsset())
	{
		FVector SourceLocation;
		GetSourceLocation(SourceLocation);
		
		if (IsSourceInAbilityReach())
		{
			TerrainPosition = SourceLocation;
			return;
		}
		
		FVector DirectionToSource = (SourceLocation - AbilityPosition).GetSafeNormal2D();
		if (DirectionToSource.IsNearlyZero()) { DirectionToSource = FVector::ForwardVector; }
		TerrainPosition = AbilityPosition + (DirectionToSource * AbilityData->DeployParams.Reach);
	}
}

void AControllerMain::ExecuteAbility()
{

	UEntities_AbilityComponent* AbilityComponent = UEntities_AbilityComponent::FindEntityAbilityComponent(HitSelectable);

	EndUpdatingAbility();

	const FEntities_BaseCommandData BaseCommandData = CreateBaseCommandData();

	if (UCommandBase* NewCommand = CreateCommand(BaseCommandData))
	{
		NewCommand->Execute();
	}

	TArray<AActor*> AbilityTargets;
	
	AbilityComponent->ExecuteAbility(AbilityTargets);
	
	AbilitySelected = FPrimaryAssetId();

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

UAbilities_NormalDataAsset* AControllerMain::GetAbilityDataAsset() const
{
	const UAssetManager* AssetManager = UAssetManager::GetIfInitialized();

	if (AbilitySelected.IsValid() || AssetManager)
	{
		return Cast<UAbilities_NormalDataAsset>(AssetManager->GetPrimaryAssetObject(AbilitySelected));
	}
	else
	{
		return nullptr;
	}
}


