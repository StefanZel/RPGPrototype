// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMain.h"
#include "Controllermain.h"
#include "RPGTests/Data/Inputs/PlayerInputDataAsset.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"


APlayerMain::APlayerMain(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 2000.f;
	SpringArmComponent->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	bIsInitialized = false;
	
}

void APlayerMain::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SpringArmComponent != nullptr && bIsInitialized)
	{
		UpdateCamera(DeltaTime);
	}
}

void APlayerMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (IsLocallyControlled())
	{
		ensureMsgf(InputDataAsset, TEXT("Input Data Asset is invalid"));

		InputData = Cast<UPlayerInputDataAsset>(InputDataAsset);

		if (InputData != nullptr) 
		{
			if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
			{
				EnhancedInputComponent->BindAction(InputData->Move, ETriggerEvent::Triggered, this, &APlayerMain::Input_Move);
				EnhancedInputComponent->BindAction(InputData->Look, ETriggerEvent::Triggered, this, &APlayerMain::Input_Look);
				EnhancedInputComponent->BindAction(InputData->Zoom, ETriggerEvent::Triggered, this, &APlayerMain::Input_Zoom);
				EnhancedInputComponent->BindAction(InputData->Rotate, ETriggerEvent::Triggered, this, &APlayerMain::Input_Rotate);
				EnhancedInputComponent->BindAction(InputData->Select, ETriggerEvent::Triggered, this, &APlayerMain::Input_Select);

				SetPawnControlDefaults();
				SetPlayerInputMode();
				SetInputDefault();
			}
		}
	}
}

void APlayerMain::SetPawnControlDefaults()
{
	if (IsLocallyControlled() && SpringArmComponent != nullptr && InputData != nullptr)
	{
		TargetLocation = GetActorLocation();

		TargetZoom = InputData->StartZoom;

		const FRotator Rotation = SpringArmComponent->GetRelativeRotation();
		TargetRotation = FRotator(Rotation.Pitch - InputData->StartPitch, Rotation.Yaw, 0.f);
		Pitch = TargetRotation.Pitch;
		Yaw = TargetRotation.Yaw;
	}

	bIsInitialized = true;
}

void APlayerMain::SetPlayerInputMode()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();

			FInputModeGameAndUI InputMode;
			InputMode.SetHideCursorDuringCapture(false);
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputMode);
			PlayerController->SetShowMouseCursor(true);
		}
	}
}

void APlayerMain::SetInputDefault(const bool bEnabled)
{
	if (InputData != nullptr && InputData->MappingContextDefault)
	{
		bEnabled ? AddInputMapping(InputData->MappingContextDefault, InputData->MapPriorityDefault) : RemoveInputMapping(InputData->MappingContextDefault);
	}
}

void APlayerMain::AddInputMapping(const UInputMappingContext* InputMappingContext, const int32 MappingPriority) const
{
	if (InputMappingContext != nullptr)
	{
		if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				if (!Subsystem->HasMappingContext(InputMappingContext)) 
				{
					Subsystem->AddMappingContext(InputMappingContext, MappingPriority);
				}
			}
		}
	}
}

void APlayerMain::RemoveInputMapping(const UInputMappingContext* InputMappingContext) const
{
	if (InputMappingContext != nullptr)
	{
		if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->RemoveMappingContext(InputMappingContext);
			}
		}
	}
}


void APlayerMain::UpdateCamera(const float DeltaTime)
{
	if (SpringArmComponent != nullptr && InputData != nullptr) 
	{
		const FVector InterpLocation = FMath::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, InputData->Smoothing);
		SetActorLocation(InterpLocation);

		const float InterpZoom = FMath::FInterpTo(SpringArmComponent->TargetArmLength, TargetZoom, DeltaTime, InputData->Smoothing);
		SpringArmComponent->TargetArmLength = InterpZoom;

		const FRotator InterpRotation = FMath::RInterpTo(SpringArmComponent->GetRelativeRotation(), TargetRotation, DeltaTime, InputData->Smoothing);
		SpringArmComponent->SetRelativeRotation(InterpRotation);
	}
}

void APlayerMain::Input_Move(const FInputActionValue& InputActionValue)
{
	if (SpringArmComponent != nullptr && InputData != nullptr && ensure(InputActionValue.GetValueType() == EInputActionValueType::Axis2D))
	{
		const FVector2d Value = InputActionValue.Get<FVector2d>();
		const float Speed = InputData->MoveSpeed;
		TargetLocation += SpringArmComponent->GetTargetRotation().RotateVector(FVector(Value.X * Speed, Value.Y * Speed, 0.0f));

		if (const AControllerMain* PlayerController = Cast<AControllerMain>(GetController()))
		{
			PlayerController->GetTerrainPosition(TargetLocation);
		}
	}
}

void APlayerMain::Input_Look(const FInputActionValue& InputActionValue)
{
	if(InputData != nullptr && ensure(InputActionValue.GetValueType() == EInputActionValueType::Axis1D))
	{ 
		const float Speed = InputData->RotateSpeed;
		const float Value = InputActionValue.Get<float>() * Speed * 0.25;
		Pitch = FMath::Clamp(Pitch + Value, InputData->PitchMin, InputData->PitchMax);
		const FQuat YawQuat(FVector::UpVector, FMath::DegreesToRadians(Yaw));
		const FQuat PitchQuat(FVector::RightVector, FMath::DegreesToRadians(Pitch));
		FRotator NewRotation = (YawQuat + PitchQuat).Rotator();
		NewRotation.Roll = 0.f;
		TargetRotation = NewRotation;
	}
}

void APlayerMain::Input_Zoom(const FInputActionValue& InputActionValue)
{
	if (InputData != nullptr && ensure(InputActionValue.GetValueType() == EInputActionValueType::Axis1D))
	{
		const float Speed = InputData->ZoomSpeed;
		TargetZoom = FMath::Clamp(TargetZoom + InputActionValue.Get<float>() * Speed, InputData->MinZoom, InputData->MaxZoom);

	}
}

void APlayerMain::Input_Rotate(const FInputActionValue& InputActionValue)
{
	if (InputData != nullptr && ensure(InputActionValue.GetValueType() == EInputActionValueType::Axis1D))
	{
		const float Speed = InputData->RotateSpeed;
		const float Value = InputActionValue.Get<float>() * Speed * 0.25;
		Yaw += Value;
		TargetRotation = FRotator(TargetRotation.Pitch, TargetRotation.Yaw + Value, 0.f);
	}
}

void APlayerMain::Input_Select(const FInputActionValue& InputActionValue)
{

}


void APlayerMain::AddGold()
{
	gold += 10;

	UE_LOG(LogTemp, Warning, TEXT("Added gold %d"), gold);

}
void APlayerMain::RemoveGold()
{
	gold -= 10;

	UE_LOG(LogTemp, Warning, TEXT("Removed gold %d"), gold);

}

