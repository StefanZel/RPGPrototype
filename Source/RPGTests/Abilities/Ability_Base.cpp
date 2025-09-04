

#include "Ability_Base.h"
#include "RPGTests/Managers/ControllerMain.h"
#include "RPGTests/Component/Entities_Component.h"

AAbility_Base::AAbility_Base(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CustomCollisionMesh"));
	RootComponent = MeshComponent;

	MeshComponent->SetCollisionProfileName(TEXT("Spells"));

	MeshComponent->SetGenerateOverlapEvents(true);

	MeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AAbility_Base::OnOverlapBegin);
	MeshComponent->OnComponentEndOverlap.AddDynamic(this, &AAbility_Base::OnOverlapEnd);
}

void AAbility_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAbility_Base::InitializeAbility(const FPrimaryAssetId& AbilityData)
{
}

void AAbility_Base::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (UEntities_Component* EntityComponent = UEntities_Component::FindEntityComponent(OtherActor))
		{
			EntityComponent->Highlight(true);
		}
	}
}

void AAbility_Base::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (UEntities_Component* EntityComponent = UEntities_Component::FindEntityComponent(OtherActor))
		{
			EntityComponent->Highlight(false);
		}
	}
}

void AAbility_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

