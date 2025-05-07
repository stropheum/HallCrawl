// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaserCannon.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class ALaserCannon : public AActor
{
	GENERATED_BODY()

public:
	ALaserCannon();
	
	UFUNCTION(BlueprintCallable)
	void ActivateBeam(bool bActive);

	UFUNCTION(BlueprintCallable)
	UNiagaraComponent* GetNiagaraEffect() const { return NiagaraEffect; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	UNiagaraComponent* NiagaraEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	UNiagaraSystem* NiagaraSystemAsset;

private:
	void InitializeNiagaraSystem() const;
	void PerformRaycast() const;

	bool bIsFiring = false;
};