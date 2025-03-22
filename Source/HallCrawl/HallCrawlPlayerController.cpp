#include "HallCrawlPlayerController.h"
#include "Blueprint/UserWidget.h"


void AHallCrawlPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ensureMsgf(EquippedWeaponWidget, TEXT("EquippedWeaponWidget != nullptr"));
	EquippedWeaponWidget->AddToViewport();
}

void AHallCrawlPlayerController::SetEquippedWeaponHudMaterial_Implementation(UMaterial* NewMaterial)
{
	if (!ensureMsgf(EquippedWeaponWidget, TEXT("EquippedWeaponWidget != nullptr"))) { return; }
	EquippedWeaponWidget->SetWeaponImageMaterial(NewMaterial);
}
