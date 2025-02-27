// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstTestCharacter.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RPGTests/Component/Entities_Component.h"
#include "Perception/AISense_Sight.h"



AFirstTestCharacter::AFirstTestCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	//GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	//GetCapsuleComponent()->SetRelativeLocation(FVector(0.f, 0.f, -96.f));
	
	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	
	UCapsuleComponent* Capsule = GetCapsuleComponent();
    	if (Capsule)
    	{
        	RootComponent = Capsule; 
    	}
	
	CharacterMoveComponent = GetCharacterMovement();
	
	if (CharacterMoveComponent)
	{
		CharacterMoveComponent->SetMovementMode(EMovementMode::MOVE_Walking);
		CharacterMoveComponent->bOrientRotationToMovement = true;
		CharacterMoveComponent->RotationRate = FRotator(0.f, 640.f, 0.f);
		CharacterMoveComponent->bConstrainToPlane = true;
		CharacterMoveComponent->bSnapToPlaneAtStart = true;
		CharacterMoveComponent->bUseFlatBaseForFloorChecks = true;
		CharacterMoveComponent->GravityScale = 1.f;

		MaxMovementSpeed = GetMaxSpeed();
	}
	//GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -96.f));
}

void AFirstTestCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void AFirstTestCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (!RootComponent)
	{
		USceneComponent* SceneLayout = NewObject<USceneComponent>(this, TEXT("SceneLayout"));

		SceneLayout->RegisterComponent();
		SceneLayout->OnComponentCreated();

		SetRootComponent(SceneLayout);
	}
}

void AFirstTestCharacter::CreatePerceptionStimuliSourceComponent()
{
	StimuliSourceComponent = NewObject<UAIPerceptionStimuliSourceComponent>(this, TEXT("StimuliSourceComponent"));

	if (StimuliSourceComponent != nullptr)
	{
		StimuliSourceComponent->RegisterForSense(UAISense_Sight::StaticClass());
		StimuliSourceComponent->RegisterWithPerceptionSystem();
	}
}

void AFirstTestCharacter::CommandMove(const FEntities_Navigation NavigationData)
{

}


void AFirstTestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFirstTestCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	Super::GetActorEyesViewPoint(OutLocation, OutRotation);

	OutLocation = GetMesh()->GetSocketLocation("headSocket");
	OutRotation = FRotator(0.f, GetMesh()->GetSocketRotation("headSocket").Yaw, 0.f);
}

float AFirstTestCharacter::GetMaxSpeed()
{
	if (UEntities_Component* Entity = UEntities_Component::FindEntityComponent(this))
	{
		if (UEntities_DataAssetMain* EntityData = Entity->GetData())
		{
			return EntityData->MaxSpeed;
		}
	}

	return 200.f;
}



