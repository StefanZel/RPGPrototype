// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "PlayerMain.generated.h"

class UInputMappingContext;
class UPlayerInputDataAsset;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class RPGTESTS_API APlayerMain : public APawn
{
	GENERATED_BODY()

public:

	APlayerMain(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	int gold;

	UFUNCTION()
	void AddGold();

	UFUNCTION()
	void RemoveGold();

protected:

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void SetPawnControlDefaults();
	virtual void SetPlayerInputMode();

	virtual void SetInputDefault(const bool bEnabled = true);
	virtual void AddInputMapping(const UInputMappingContext* InputMappingContext, const int32 MappingPriority) const;
	virtual void RemoveInputMapping(const UInputMappingContext* InputMappingContext) const;

	void UpdateCamera(const float DeltaTime);

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Player Input Settings")
	UDataAsset* InputDataAsset;

	UPROPERTY()
	UPlayerInputDataAsset* InputData;

	/** Input Functions **/
	virtual void Input_Move(const FInputActionValue& InputActionValue);
	virtual void Input_Look(const FInputActionValue& InputActionValue);
	virtual void Input_Zoom(const FInputActionValue& InputActionValue);
	virtual void Input_Rotate(const FInputActionValue& InputActionValue);
	virtual void Input_Select(const FInputActionValue& InputActionValue);



private:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

	UPROPERTY();
	bool bIsInitialized;

	UPROPERTY();
	FVector TargetLocation;

	UPROPERTY();
	FRotator TargetRotation;

	UPROPERTY();
	float Pitch;

	UPROPERTY();
	float Yaw;

	UPROPERTY();
	float TargetZoom;


};
