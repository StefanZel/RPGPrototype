// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities_AiControllerMain.h"
#include "Engine/AssetManager.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "RPGTests/Interfaces/Entities_Interface.h"


AEntities_AiControllerMain::AEntities_AiControllerMain(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void AEntities_AiControllerMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEntities_AiControllerMain::BeginPlay()
{
	Super::BeginPlay();
}

void AEntities_AiControllerMain::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	InitAiOnGameplay();
}

void AEntities_AiControllerMain::InitAiOnGameplay()
{
	if (AiDataAssetId.IsValid())
	{
		InitBehavior();

		InitPerception();

		CreateStimuliSourceComponent();
	}
}

UAi_DataAssetMain* AEntities_AiControllerMain::GetAiData() const
{
	if (AiDataAssetId.IsValid())
	{
		if (const UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
		{
			return Cast<UAi_DataAssetMain>(AssetManager->GetPrimaryAssetObject(AiDataAssetId));
		}
	}
	return nullptr;
}

void AEntities_AiControllerMain::InitBehavior()
{
	if (UAi_DataAssetMain* AiData = GetAiData())
	{
		if (AiData->BehaviorTreeAsset.LoadSynchronous())
		{
			BehaviorTreeAsset = AiData->BehaviorTreeAsset.LoadSynchronous();
		}

		if (BehaviorTreeAsset != nullptr && BehaviorTreeAsset->BlackboardAsset != nullptr)
		{
			UBlackboardComponent* NewBlackboard;
			UseBlackboard(BehaviorTreeAsset->BlackboardAsset, NewBlackboard);
			Blackboard = NewBlackboard;

			RunBehaviorTree(BehaviorTreeAsset);
		}
	}
}

void AEntities_AiControllerMain::InitPerception()
{
	InitSightPerception();
}

void AEntities_AiControllerMain::InitSightPerception()
{
	if (UAi_DataAssetMain* AiData = GetAiData())
	{
		PerceptionComponent = NewObject<UAIPerceptionComponent>(this, TEXT("AIPerceptionComponent"));
		SightConfig = NewObject<UAISenseConfig_Sight>(this, TEXT("SightConfig"));

		if (SightConfig && GetPerceptionComponent())
		{
			SightConfig->SightRadius = AiData->SightRadius;
			SightConfig->LoseSightRadius = SightConfig->SightRadius + AiData->LoseSightRadius;
			SightConfig->PeripheralVisionAngleDegrees = AiData->VisionAngleDegree;
			SightConfig->SetMaxAge(AiData->SightAge);
			SightConfig->AutoSuccessRangeFromLastSeenLocation = AiData->AutoSuccessRangeFromLastSeenLocation;
			SightConfig->DetectionByAffiliation.bDetectEnemies = AiData->DetectEnemies;
			SightConfig->DetectionByAffiliation.bDetectFriendlies = AiData->DetectAllies;
			SightConfig->DetectionByAffiliation.bDetectNeutrals = AiData->DetectNeutral;

			GetPerceptionComponent()->ConfigureSense(*SightConfig);
			GetPerceptionComponent()->SetDominantSense(SightConfig->GetSenseImplementation());
			GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &ThisClass::UpdatePercievedActors);
			GetPerceptionComponent()->RequestStimuliListenerUpdate();
			GetPerceptionComponent()->SetSenseEnabled(UAISense_Sight::StaticClass(), true);
		}
	}
}

void AEntities_AiControllerMain::CreateStimuliSourceComponent() const
{
	if (GetPawn())
	{
		if (IEntities_Interface* EntityInterface = Cast<IEntities_Interface>(GetPawn()))
		{
			EntityInterface->CreatePerceptionStimuliSourceComponent();
		}
	}
}

void AEntities_AiControllerMain::StartUpdatingPerception()
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(Handle_PerceptionUpdate))
	{
		GetWorld()->GetTimerManager().SetTimer(Handle_PerceptionUpdate, this, &ThisClass::UpdatePerception, 2.0f, true);
	}
}

void AEntities_AiControllerMain::EndUpdatingPerception()
{
	GetWorld()->GetTimerManager().ClearTimer(Handle_PerceptionUpdate);
}

void AEntities_AiControllerMain::UpdatePerception()
{
	for (auto It = PercievedEntities.CreateIterator(); It; ++It)
	{
		if (It.Key() != nullptr)
		{
			const float SeenTime = GetWorld()->GetTimeSeconds();


			if (HasLineOfSight(It.Key()))
			{
				It.Value() = SeenTime;

				// @TODO: Add the ability to percieve allies, traps, etc.
			}
			else
			{
				if (SeenTime - It.Value() > SightConfig->GetMaxAge())
				{
					It.RemoveCurrent();
					continue;
				}
			}
		}
	}

	if (PercievedEntities.Num() == 0)
	{
		EndUpdatingPerception();
	}

}

bool AEntities_AiControllerMain::HasLineOfSight(const AActor* Target) const
{
	if (GetPawn())
	{
		FVector TargetLocation;
		FRotator TargetViewRotation;
		Target->GetActorEyesViewPoint(TargetLocation, TargetViewRotation);


		FVector StartLocation;
		FRotator ViewRotation;
		GetPawn()->GetActorEyesViewPoint(StartLocation, ViewRotation);

		FCollisionQueryParams CollisionParams(SCENE_QUERY_STAT(LineOfSight), true, GetPawn());
		CollisionParams.AddIgnoredActor(GetPawn());

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, TargetLocation, ECC_Visibility, CollisionParams))
		{
			if (Hit.bBlockingHit)
			{
				if (Hit.GetActor() == Target)
				{
					return true;
				}
			}
		}
	}

	return false;
}

void AEntities_AiControllerMain::UpdatePercievedActors(const TArray<AActor*>& UpdatedActors)
{
	for (int i = 0; i < UpdatedActors.Num(); i++)
	{
		if (!PercievedEntities.Contains(UpdatedActors[i]))
		{
			PercievedEntities.Add(UpdatedActors[i], GetWorld()->GetTimeSeconds());
		}
	}
	
	if (PercievedEntities.Num() > 0)
	{
		StartUpdatingPerception();
	}
	else
	{
		EndUpdatingPerception();
	}
}

