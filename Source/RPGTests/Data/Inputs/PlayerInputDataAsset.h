// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "PlayerInputDataAsset.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class RPGTESTS_API UPlayerInputDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	/** Control Inputs **/

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Control Parameters")
	float MoveSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Control Parameters")
	float EdgeScrollSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Control Parameters")
	float RotateSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Control Parameters")
	float PitchMin;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Control Parameters")
	float PitchMax;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Control Parameters")
	float StartPitch;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Control Parameters")
	float StartZoom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Control Parameters")
	float MinZoom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Control Parameters")
	float MaxZoom;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Control Parameters")
	float ZoomSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Control Parameters")
	float Smoothing;

	/** Mapping Input Context **/

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mapping Context|Default")
	UInputMappingContext* MappingContextDefault;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mapping Context|Default")
	int32 MapPriorityDefault;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mapping Context|Default")
	UInputAction* Move;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mapping Context|Default")
	UInputAction* Look;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mapping Context|Default")
	UInputAction* Zoom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mapping Context|Default")
	UInputAction* Rotate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mapping Context|Default")
	UInputAction* Select;


};
