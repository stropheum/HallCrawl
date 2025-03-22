#pragma once

#include "CoreMinimal.h"
#include "EquippedWeaponWidget.h"
#include "GameFramework/PlayerController.h"
#include "HallCrawlPlayerController.generated.h"

class UImage;

UCLASS()
class HALLCRAWL_API AHallCrawlPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=HallCrawlPlayerController, meta=(AllowPrivateAccess=true))
	UEquippedWeaponWidget* EquippedWeaponWidget = nullptr;

public:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintNativeEvent, Category=HallCrawlPlayerController, meta=(AllowPrivateAccess=true))
	void SetEquippedWeaponHudMaterial(UMaterial* NewMaterial);

};
