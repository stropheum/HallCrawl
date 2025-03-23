#include "NeedlerProjectile.h"

#include "PhysicsAssetUtils.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


ANeedlerProjectile::ANeedlerProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ANeedlerProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, const FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		ImpactComponent = OtherComp;
		ImpactVelocity = GetVelocity() * 100.0f;
		
		ProjectileMovement->StopMovementImmediately();
		CollisionComp->Deactivate();
		RootComponent->AttachToComponent(OtherComp, FAttachmentTransformRules::KeepWorldTransform, NAME_None);
		
		HasHit = true;
	}
}

void ANeedlerProjectile::BeginPlay()
{
	Super::BeginPlay();
	UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(GetRootComponent());
	if (!ensureMsgf(RootComp, TEXT("Root Component failed to cast to UPrimitiveComponent"))) { return; }
	
	RootComp->SetCollisionObjectType(ECC_GameTraceChannel1);
	RootComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
}

void ANeedlerProjectile::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasHit)
	{
		if (!ImpactComponent.IsValid())
		{
			Destroy();
		}
		
		ElapsedTimeSinceHit += DeltaTime;
		if (ElapsedTimeSinceHit >= ExplosionDelay)
		{
			ImpactComponent->AddImpulseAtLocation(ImpactVelocity, GetActorLocation());
			HasHit = false;
			Destroy();
		}
	}
}
