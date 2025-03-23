#include "NeedlerProjectile.h"

#include "Components/SphereComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"


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

void ANeedlerProjectile::ExplodeFromSource(ANeedlerProjectile* ProjectileExplosionSource)
{
	if (ImpactComponent.IsValid())
	{
		ImpactComponent->AddImpulseAtLocation(ImpactVelocity, GetActorLocation());	
	}
	
	HasHit = false;

	if (ExplosionNiagaraSystem)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ExplosionNiagaraSystem,
			GetActorLocation(),
			GetActorRotation(),
			FVector(1.f, 1.f, 1.f),
			true);
	}
	
	if (ProjectileExplosionSource == this && ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
	}

	Destroy();

	if (const UWorld* World = GetWorld())
	{
		TArray<FOverlapResult> OverlapResults;
		FCollisionShape CollisionShape;
		CollisionShape.SetSphere(ExplosionRadius);

		World->OverlapMultiByChannel(
			OverlapResults, GetActorLocation(), FQuat::Identity, ECC_GameTraceChannel1, CollisionShape);

		for (const FOverlapResult& Overlap : OverlapResults)
		{
			if (ANeedlerProjectile* OtherProjectile = Cast<ANeedlerProjectile>(Overlap.GetActor());
				OtherProjectile && OtherProjectile != this && !OtherProjectile->IsPendingKillPending())
			{
				OtherProjectile->ExplodeFromSource(this);
			}
		}
	}
}

void ANeedlerProjectile::BeginPlay()
{
	Super::BeginPlay();
	UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(GetRootComponent());
	if (!ensureMsgf(RootComp, TEXT("Root Component failed to cast to UPrimitiveComponent"))) { return; }

	RootComp->SetCollisionObjectType(ECC_GameTraceChannel1);
	RootComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
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
			ExplodeFromSource(this);
		}
	}
}
