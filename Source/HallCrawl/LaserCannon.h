// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaserCannon.generated.h"


UCLASS()
class HALLCRAWL_API ALaserCannon : public AActor
{
	GENERATED_BODY()

public:
	ALaserCannon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	class UNiagaraSystem* NiagaraSystemAsset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	class UNiagaraComponent* NiagaraEffect;


protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	
	void AlignToNearestActor(const float DeltaTime);
	
	void PerformRaycast() const;

	void InitializeNiagaraSystem() const;

	void Fire();

	bool GetMouseClickPosition(FVector& OutHitLocation) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LaserBeamComponent", meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	UPROPERTY()
	APlayerController* PlayerController;

	FRandomStream RandomStream;
	float TimeSinceLastTargetSelection;
};
