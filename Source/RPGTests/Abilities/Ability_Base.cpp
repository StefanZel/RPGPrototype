

#include "Ability_Base.h"
#include "RPGTests/Data/AStaticGameData.h"
#include "Engine/AssetManager.h"
#include "RPGTests/Component/Entities_Component.h"

AAbility_Base::AAbility_Base(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CustomCollisionMesh"));
	RootComponent = MeshComponent;

	MeshComponent->SetCollisionProfileName(TEXT("Spells"));
	
	NiagaraEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("AbilityEffect"));
	NiagaraEffect->SetupAttachment(RootComponent);
	NiagaraEffect->bAutoActivate = false;
}

void AAbility_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

UAbilities_NormalDataAsset* AAbility_Base::GetAbilityData() 
{	
	const UAssetManager* AssetManager = UAssetManager::GetIfInitialized();

	if (AssetManager)
	{
		return Cast<UAbilities_NormalDataAsset>(AssetManager->GetPrimaryAssetObject(ThisAbilityData));
	}
	else
	{
		return nullptr;
	}
}

float AAbility_Base::GetAbilityRange()
{
	float DefaultRange = 1.f;

	if (const UAbilities_NormalDataAsset* Data = GetAbilityData())
	{
		return Data->XRange;
	}
	return DefaultRange;

}

void AAbility_Base::CheckForTargets()
{
	//DrawDebugBox(GetWorld(), MeshComponent->GetComponentLocation(), MeshComponent->Bounds.BoxExtent, MeshComponent->GetComponentQuat(), FColor::Green, false, 0.1f);
	TArray<FOverlapResult> Overlaps;

	FVector Position = MeshComponent->GetComponentLocation();
	FQuat Rotation = MeshComponent->GetComponentQuat();

	FCollisionShape MeshShape = MeshComponent->GetCollisionShape();

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHasHits = GetWorld()->OverlapMultiByChannel(Overlaps, Position, Rotation, TRACE_CHANNEL_ENTITY, MeshShape, Params);

	TSet<TWeakObjectPtr<AActor>> NewTargets;

	for (const FOverlapResult& Result : Overlaps)
	{
		if (AActor* OverlappedActor = Result.GetActor())
		{
			if (auto* EntityComponent = UEntities_Component::FindEntityComponent(OverlappedActor))
			{
				NewTargets.Add(OverlappedActor);
				if (!ActiveTargets.Contains(OverlappedActor))
				{
					EntityComponent->Highlight(true);
				}
			}
		}
	}

	for (auto It = ActiveTargets.CreateIterator(); It; ++It)
	{
		if (!NewTargets.Contains(*It))
		{
			if (It->IsValid())
			{
				if (UEntities_Component* EntityComponent = UEntities_Component::FindEntityComponent(It->Get()))
				{
				EntityComponent->Highlight(false);
				}
			}
			It.RemoveCurrent();
		}
	}

	ActiveTargets = NewTargets;
}

void AAbility_Base::SetEffectSize()
{
	if (MeshComponent && NiagaraEffect)
	{
		FVector MeshVector = MeshComponent->GetStaticMesh()->GetBounds().BoxExtent;
		FVector EffectSize = MeshVector * 2.0f;
		NiagaraEffect->SetVariableVec3(FName("User.DecalSize"), EffectSize);
	}
}

void AAbility_Base::InitializeAbility(const FPrimaryAssetId& AbilityData)
{
	ThisAbilityData = AbilityData;

	UAbilities_NormalDataAsset* DataAsset = GetAbilityData();

	MeshComponent->SetStaticMesh(DataAsset->MeshComponent.LoadSynchronous());
	MeshComponent->SetVisibleInRayTracing(false);
	MeshComponent->SetHiddenInGame(true);

	UNiagaraSystem* NiagaraSystem = DataAsset->NiagaraEffect.LoadSynchronous();

	if (NiagaraSystem)
	{
		NiagaraEffect->SetAsset(NiagaraSystem);
		SetEffectSize();
		NiagaraEffect->Activate();
	}

}

void AAbility_Base::UpdateAbility(const FVector& Position)
{
	SetActorLocation(Position);

	CheckForTargets();
}

TArray<AActor*> AAbility_Base::GetTargetActorsOnExecute()
{
	TArray<AActor*> Targets;
	
	for (const TWeakObjectPtr<AActor>& WeakActor : ActiveTargets)
	{
		if (AActor* RawActor = WeakActor.Get())
		{
			Targets.Add(RawActor);
		}
	}
	return Targets;
}

void AAbility_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

