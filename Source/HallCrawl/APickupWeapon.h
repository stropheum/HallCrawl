#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "APickupWeapon.generated.h"

UCLASS()
class HALLCRAWL_API AAPickupWeapon : public AActor
{
	GENERATED_BODY()

public:
	AAPickupWeapon();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PickupWeapon, meta = (AllowPrivateAccess = "true"))
	USceneCaptureComponent2D* SceneCaptureComponent2D = nullptr;

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
