#include "LaserCannon.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

ALaserCannon::ALaserCannon()
{
	NiagaraEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraEffect"));
	RootComponent = NiagaraEffect;
	PrimaryActorTick.bCanEverTick = true;
}

void ALaserCannon::BeginPlay()
{
	Super::BeginPlay();
	InitializeNiagaraSystem();
}

void ALaserCannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (NiagaraEffect && NiagaraEffect->IsActive())
	{
		PerformRaycast();
	}
}

void ALaserCannon::ActivateBeam(const bool bActive)
{
	if (NiagaraEffect)
	{
		if (bActive)
		{
			NiagaraEffect->Activate(true);
		}
		else
		{
			NiagaraEffect->Deactivate();
		}
	}
}

void ALaserCannon::InitializeNiagaraSystem() const
{
	if (NiagaraEffect && NiagaraSystemAsset)
	{
		NiagaraEffect->SetAsset(NiagaraSystemAsset);
		NiagaraEffect->SetRelativeLocation(FVector::ZeroVector);
		NiagaraEffect->Deactivate();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NiagaraSystemAsset or NiagaraEffect is null!"));
	}
}

void ALaserCannon::PerformRaycast() const
{
	const FVector Start = GetActorLocation();
	const FVector ForwardVector = GetActorForwardVector();
	constexpr float RayLength = 10000.0f;
	const FVector End = Start + (ForwardVector * RayLength);

	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	if (const APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn())
	{
		QueryParams.AddIgnoredActor(PlayerPawn);
	}
	
	const FCollisionObjectQueryParams ObjectParams(ECC_Visibility | ECC_WorldDynamic | ECC_WorldStatic | ECC_PhysicsBody);

	const bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectParams, QueryParams);
	const FVector BeamEnd = bHit ? Hit.ImpactPoint : End;

	NiagaraEffect->SetVectorParameter(TEXT("Beam End"), BeamEnd);
}
