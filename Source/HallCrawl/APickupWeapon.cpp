#include "APickupWeapon.h"
#include "Components/SceneCaptureComponent2D.h"


AAPickupWeapon::AAPickupWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent2D"));
	SceneCaptureComponent2D->SetRelativeLocation(FVector(-60, 10, 0));
	SceneCaptureComponent2D->SetupAttachment(RootComponent);
}

void AAPickupWeapon::BeginPlay()
{
	Super::BeginPlay();
	SceneCaptureComponent2D->ShowOnlyActorComponents(this);
	SceneCaptureComponent2D->CaptureScene();
}

void AAPickupWeapon::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

