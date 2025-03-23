#include "NeedlerProjectile.h"


ANeedlerProjectile::ANeedlerProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ANeedlerProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ANeedlerProjectile::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

