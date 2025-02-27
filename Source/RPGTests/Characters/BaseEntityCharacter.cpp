


#include "BaseEntityCharacter.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RPGTests/Component/Entities_Component.h"
#include "Perception/AISense_Sight.h"


ABaseEntityCharacter::ABaseEntityCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

	PrimaryActorTick.bCanEverTick = true;

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
	}

}

void ABaseEntityCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	Super::GetActorEyesViewPoint(OutLocation, OutRotation);

	OutLocation = GetMesh()->GetSocketLocation("headSocket");
	OutRotation = FRotator(0.f, GetMesh()->GetSocketRotation("headSocket").Yaw, 0.f);
}


void ABaseEntityCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseEntityCharacter::CreatePerceptionStimuliSourceComponent()
{
	StimuliSourceComponent = NewObject<UAIPerceptionStimuliSourceComponent>(this, TEXT("StimuliSourceComponent"));

	if (StimuliSourceComponent != nullptr)
	{
		StimuliSourceComponent->RegisterForSense(UAISense_Sight::StaticClass());
		StimuliSourceComponent->RegisterWithPerceptionSystem();
	}
}

// Called every frame
void ABaseEntityCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

