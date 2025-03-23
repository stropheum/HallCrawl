#pragma once

#include "CoreMinimal.h"
#include "HallCrawlProjectile.h"
#include "NeedlerProjectile.generated.h"

UCLASS()
class HALLCRAWL_API ANeedlerProjectile : public AHallCrawlProjectile
{
	GENERATED_BODY()

public:
	ANeedlerProjectile();

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	                   UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	float ExplosionDelay = 1.5f;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	FVector ImpactVelocity = FVector::Zero();
	float ElapsedTimeSinceHit = 0.0f;
	TWeakObjectPtr<UPrimitiveComponent> ImpactComponent;
	bool HasHit = false;
};
