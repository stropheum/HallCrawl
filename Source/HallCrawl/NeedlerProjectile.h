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

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
