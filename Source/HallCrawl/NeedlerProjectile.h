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

	UFUNCTION()
	void Explode();


protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void ResetNeedlesOnTarget(const AActor* Target);

	UFUNCTION()
	void TickBackExplosionTimer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Needler, meta=(AllowPrivateAccess=true))
	float ExplosionDelay = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Needler, meta=(AllowPrivateAccess=true))
	float AccumulationTickBack = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Needler, meta=(AllowPrivateAccess=true))
	float ExplosionRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Needler, meta=(AllowPrivateAccess=true))
	class UNiagaraSystem* ExplosionNiagaraSystem = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Needler, meta=(AllowPrivateAccess=true))
	USoundBase* ExplosionSound = nullptr;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	FVector ImpactVelocity = FVector::Zero();
	float ElapsedTimeSinceHit = 0.0f;
	TWeakObjectPtr<UPrimitiveComponent> ImpactComponent;
	bool HasHit = false;
	bool IsFirstToHitTarget = false;
};