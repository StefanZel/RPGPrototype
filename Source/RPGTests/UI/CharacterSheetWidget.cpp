// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSheetWidget.h"

#include "RPGTests/Component/Entities_StatComponent.h"

void UCharacterSheetWidget::InitForActor(AActor* Actor)
{
	UnbindDelegates();
	
	CachedStatComponent = UEntities_StatComponent::FindEntityStatComponent(Actor);
	if (!CachedStatComponent) return;
	
	InitializeWithComponent(CachedStatComponent);
}

void UCharacterSheetWidget::NativeDestruct()
{
	UnbindDelegates();
	Super::NativeDestruct();
}

void UCharacterSheetWidget::InitializeWithComponent(UEntities_StatComponent* StatComponent)
{
	StatComponent->OnStatChanged.AddDynamic(this, &UCharacterSheetWidget::HandleStatChanged);
	StatComponent->OnResourceChanged.AddDynamic(this, &UCharacterSheetWidget::HandleResourceChanged);
	StatComponent->OnModifierApplied.AddDynamic(this, &UCharacterSheetWidget::HandleModifierApplied);
	StatComponent->OnModifierRemoved.AddDynamic(this, &UCharacterSheetWidget::HandleModifierRemoved);
	
	BP_OnInitialized(StatComponent);
}

void UCharacterSheetWidget::UnbindDelegates()
{
	if (!CachedStatComponent) return;
	CachedStatComponent->OnStatChanged.RemoveAll(this);
	CachedStatComponent->OnResourceChanged.RemoveAll(this);
	CachedStatComponent->OnModifierApplied.RemoveAll(this);
	CachedStatComponent->OnModifierRemoved.RemoveAll(this);
	CachedStatComponent = nullptr;
}

void UCharacterSheetWidget::HandleStatChanged(FGameplayTag StatTag, float OldValue, float NewValue)
{
	BP_OnStatChanged(StatTag, OldValue, NewValue);
}

void UCharacterSheetWidget::HandleResourceChanged(FGameplayTag ResourceTag, float Current, float Max)
{
	BP_OnResourceChanged(ResourceTag, Current, Max);
}

void UCharacterSheetWidget::HandleModifierApplied(const FActiveModifier& Modifier)
{
	BP_OnModifiersChanged();
}

void UCharacterSheetWidget::HandleModifierRemoved(const FActiveModifier& Modifier)
{
	BP_OnModifiersChanged();
}
