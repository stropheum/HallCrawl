#include "HallCrawlPlayerController.h"
#include "Blueprint/UserWidget.h"


void AHallCrawlPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(Hud);
	Hud->AddToViewport();
}
