// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "PlayerInputDataAsset.generated.h"

class UInputMappingContext;
class UInputAction;

USTRUCT(BlueprintType)
struct FAbilityInputData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* AbilityInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 AbilityId;
};

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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mapping Context|Default")
	UInputAction* Command;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability Selection")
	TArray<FAbilityInputData> AbilityInput;
};

namespace InputActions
{
	template<class T, class FuncType>
	void Bind_StartTriggerComplete(UEnhancedInputComponent* EnhancedInputComponent, const UInputAction* Action, T* Obj, FuncType StartFunc, FuncType TriggerFunc, FuncType CompleteFunc)
	{
		if (StartFunc != nullptr)
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Started, Obj, StartFunc);
		}

		if (TriggerFunc != nullptr)
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Triggered, Obj, TriggerFunc);
		}

		if (CompleteFunc != nullptr)
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Completed, Obj, CompleteFunc);
		}
	}

	template<class T, class FuncType>
	void Bind_TriggerComplete(UEnhancedInputComponent* EnhancedInputComponent, const UInputAction* Action, T* Obj, FuncType TriggerFunc, FuncType CompleteFunc)
	{

		if (TriggerFunc != nullptr)
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Triggered, Obj, TriggerFunc);
		}

		if (CompleteFunc != nullptr)
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Completed, Obj, CompleteFunc);
		}
	}

	template<class T, class FuncType>
	void Bind_Start(UEnhancedInputComponent* EnhancedInputComponent, const UInputAction* Action, T* Obj, FuncType StartFunc)
	{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Started, Obj, StartFunc);
	}

	template<class T, class FuncType>
	void Bind_Trigger(UEnhancedInputComponent* EnhancedInputComponent, const UInputAction* Action, T* Obj, FuncType TriggerFunc)
	{

		if (TriggerFunc != nullptr)
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Triggered, Obj, TriggerFunc);
		}
	}

	template<class T, class FuncType>
	void Bind_Completed(UEnhancedInputComponent* EnhancedInputComponent, const UInputAction* Action, T* Obj, FuncType CompleteFunc)
	{

		if (CompleteFunc != nullptr)
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Completed, Obj, CompleteFunc);
		}
	}
}