// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HallCrawlProjectile.h"
#include "NeedlerProjectile.generated.h"

UCLASS()
class HALLCRAWL_API ANeedlerProjectile : public AHallCrawlProjectile
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANeedlerProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
