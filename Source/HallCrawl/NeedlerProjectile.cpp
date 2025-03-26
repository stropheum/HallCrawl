#include "NeedlerProjectile.h"

#include "HcCharacterBase.h"
#include "Components/SphereComponent.h"
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
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr)
	{
		ImpactCharacter = Cast<AHcCharacterBase>(OtherActor);
		ImpactComponent = OtherComp;
		ImpactVector = GetVelocity();

		ProjectileMovement->StopMovementImmediately();
		CollisionComp->Deactivate();
		RootComponent->AttachToComponent(OtherComp, FAttachmentTransformRules::KeepWorldTransform, NAME_None);

		HasHit = true;
		ResetNeedlesOnTarget(OtherActor);
	}
}

void ANeedlerProjectile::Explode()
{
	if (ImpactComponent.IsValid())
	{
		if (ImpactCharacter.IsValid())
		{
			ImpactCharacter->Damage(NeedleDamage);
		}
		ImpactComponent->AddImpulseAtLocation(ImpactVector * ExplosionForce, GetActorLocation());	
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

	if (IsFirstToHitTarget)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());	
	}

	Destroy();
}

void ANeedlerProjectile::ResetNeedlesOnTarget(const AActor* Target)
{
	int NeedleCount = 0;
	TArray<AActor*> AttachedActors;
	Target->GetAttachedActors(AttachedActors);
	
	for (AActor* Actor : AttachedActors)
	{
		if (ANeedlerProjectile* const NeedlerProjectile = Cast<ANeedlerProjectile>(Actor))
		{
			NeedlerProjectile->TickBackExplosionTimer();
			NeedleCount++;
		}
	}

	if (NeedleCount == 1)
	{
		IsFirstToHitTarget = true;
	}
}

void ANeedlerProjectile::TickBackExplosionTimer()
{
	ElapsedTimeSinceHit -= AccumulationTickBack;
	if (ElapsedTimeSinceHit < 0)
	{
		ElapsedTimeSinceHit = 0;
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
			Explode();
		}
	}
}
