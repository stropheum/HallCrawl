#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HallCrawlPlayerController.generated.h"

UCLASS()
class HALLCRAWL_API AHallCrawlPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UUserWidget* Hud = nullptr;

public:
	virtual void BeginPlay() override;
};
