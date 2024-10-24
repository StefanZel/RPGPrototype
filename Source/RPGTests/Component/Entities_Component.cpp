
#include "Entities_Component.h"
#include <Engine/AssetManager.h>
#include <Components/BoxComponent.h>


UEntities_Component::UEntities_Component(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;

	EntityDataAsset = nullptr;
	EntityData = nullptr;
}


void UEntities_Component::Highlight(const bool bHighlight)
{
	if (!GetOwner())
	{
		return;
	}

	TArray<UMeshComponent*> Components;
	for (int i = 0 ; Components.Num() > i; i++)
	{
		if (bHighlight)
		{
			if (UMaterialInstance* HighlightMaterial = GetHighlightMaterial())
			{
				Components[i]->SetOverlayMaterial(HighlightMaterial);
			}
		}
		else
		{
			Components[i]->SetOverlayMaterial(nullptr);
		}
	}
}

void UEntities_Component::Select(const bool bSelect)
{
	if (!GetOwner())
	{
		return;
	}

	OnSelectedChange.Broadcast(bSelect);
}

UMaterialInstance* UEntities_Component::GetHighlightMaterial()
{
	/*if (const UEntities_DataAssetMain* Data = GetData())
	{
		return Data->HighlightMaterial.LoadSynchronous();
	}*/
	if (EntityData != nullptr)
	{
		return EntityData->HighlightMaterial.LoadSynchronous();
	}
	return nullptr;
}

UMaterialInstance* UEntities_Component::GetSelectMaterial()
{
	/*if (const UEntities_DataAssetMain* Data = GetData())
	{
		return Data->SelectedMaterial.LoadSynchronous();
	}*/

	if (EntityData != nullptr)
	{
		return EntityData->SelectedMaterial.LoadSynchronous();
	}

	return nullptr;
}

UEntities_DataAssetMain* UEntities_Component::GetData() const
{
	if (const UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
	{
		if (EntityDataAssetID.IsValid())
		{
			return Cast<UEntities_DataAssetMain>(AssetManager->GetPrimaryAssetObject(EntityDataAssetID));
		}
	}
	return nullptr;
}

EEntities_AvailableTypes UEntities_Component::GetTypes() const
{
	/*if (const UEntities_DataAssetMain* Data = GetData())
	{
		return Data->EntityType;
	}*/

	if (EntityData != nullptr)
	{
		return EntityData->EntityType;
	}
	return EEntities_AvailableTypes::None;
}

EEntities_Sizes UEntities_Component::GetSizes() const
{
	/*if (const UEntities_DataAssetMain* Data = GetData())
	{
		return Data->EntitySize;
	}*/
	if (EntityData != nullptr)
	{
		return EntityData->EntitySize;
	}

	return EEntities_Sizes::None;
}

FVector UEntities_Component::GetSizeColl()
{
	FCollisionShape CollisionShape = FCollisionShape();

	if (GetOwner())
	{

		switch (GetSizes())
		{
		case EEntities_Sizes::NormalAverage: case EEntities_Sizes::NormalSmall: case EEntities_Sizes::NormalLarge:
			if (const UCapsuleComponent* CapsuleComponent = Cast<ACharacter>(GetOwner())->GetCapsuleComponent())
			{
				float Radius, Height;
				CapsuleComponent->GetScaledCapsuleSize(Radius, Height);
				CollisionShape = FCollisionShape::MakeCapsule(Radius, Height);
			}
			else
			{
				CollisionShape = FCollisionShape::MakeCapsule(45.0f, 100.0f);
			}
			break;
		case EEntities_Sizes::FourLegsSmall: case EEntities_Sizes::FourLegsLarge:
			if (const UBoxComponent* BoxComponent = GetOwner()->GetComponentByClass<UBoxComponent>())
			{
				CollisionShape = FCollisionShape::MakeBox(BoxComponent->GetScaledBoxExtent());
			}
			break;
		default:
			break;
		}
	}
	return CollisionShape.GetExtent();
}

void UEntities_Component::BeginPlay()
{
	Super::BeginPlay();

	ensureMsgf(EntityDataAsset, TEXT("Input Data Asset is invalid"));

	EntityData = Cast<UEntities_DataAssetMain>(EntityDataAsset);
	
}