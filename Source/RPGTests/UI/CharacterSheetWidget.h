// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSheetWidget.generated.h"

struct FActiveModifier;
/**
 * 
 */
UCLASS()
class RPGTESTS_API UCharacterSheetWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void InitForActor(AActor* Actor);
	
protected:
	virtual void NativeDestruct() override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Sheet")
	void BP_OnInitialized(UEntities_StatComponent* StatComponent);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Sheet")
	void BP_OnStatChanged(FGameplayTag StatTag, float OldValue, float NewValue);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Sheet")
	void BP_OnResourceChanged(FGameplayTag ResourceTag, float Current, float Max);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Sheet")
	void BP_OnModifiersChanged();
	
	UPROPERTY(BlueprintReadOnly)
	UEntities_StatComponent* CachedStatComponent;
	
private:
	void InitializeWithComponent(UEntities_StatComponent* StatComponent);
	void UnbindDelegates();
	
	UFUNCTION()
	void HandleStatChanged(FGameplayTag StatTag, float OldValue, float NewValue);
	
	UFUNCTION()
	void HandleResourceChanged(FGameplayTag ResourceTag, float Current, float Max);
	
	UFUNCTION()
	void HandleModifierApplied(const FActiveModifier& Modifier);
	
	UFUNCTION()
	void HandleModifierRemoved(const FActiveModifier& Modifier);
};
