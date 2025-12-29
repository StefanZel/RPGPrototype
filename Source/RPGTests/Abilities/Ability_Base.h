// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RPGTests/Interfaces/Abilities_Interface.h"
#include "RPGTests/Data/Abilities/Abilities_NormalDataAsset.h"
#include "NiagaraComponent.h"
#include "Ability_Base.generated.h"


UCLASS()
class RPGTESTS_API AAbility_Base : public AActor, public IAbilities_Interface
{
	GENERATED_BODY()
	
public:	
	AAbility_Base(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UNiagaraSystem* NiagaraEffect;
	// TODO: Remember! This should take the calculated data from the ability component, not the AssetId.
	virtual void InitializeAbility(const FPrimaryAssetId& AbilityData) override;
	virtual void UpdateAbility(const FVector& Position) override;
private:
	UPROPERTY()
	FPrimaryAssetId ThisAbilityData;
protected:
	virtual void BeginPlay() override;
	UAbilities_NormalDataAsset* GetAbilityData();
	float GetAbilityRange();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:	
	virtual void Tick(float DeltaTime) override;

};
