
#include "Entities_DecalComponent.h"
#include "RPGTests/Component/Entities_Component.h"



UEntities_DecalComponent::UEntities_DecalComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UEntities_DecalComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateLocation();
}


void UEntities_DecalComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!GetOwner())
	{
		return;
	}

	if (UEntities_Component* EntityComponent = UEntities_Component::FindEntityComponent(GetOwner()))
	{
		if (UMaterialInstance* SelectedMaterial = EntityComponent->GetSelectMaterial())
		{
			EntityComponent->OnSelectedChange.AddUObject(this, &ThisClass::UpdateSelected);

			const FVector Size = EntityComponent->GetSizeColl();
			const float MaxRadius = FMath::Max(Size.X, Size.Y);
			DecalSize = FVector(Size.Z, MaxRadius+20.f, MaxRadius+20.f);
			SetRelativeRotation(FRotator(90.f, 0.f, 0.f));

			UpdateLocation();

			DecalMaterial = SelectedMaterial;
			MarkRenderStateDirty();

			SetVisibility(false);
		}
	}

	
}

void UEntities_DecalComponent::UpdateSelected(const bool bSelected)
{
	if (DecalMaterial != nullptr)
	{
		SetVisibility(bSelected);
	}

}

void UEntities_DecalComponent::UpdateLocation()
{
	FVector DecalLocation = GetOwner()->GetActorLocation();
	DecalLocation.Z = 0.f;
	SetRelativeLocation(DecalLocation);
}

