#include "LaserCannon.h"

#include "HcCharacterBase.h"
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
	if (bIsFiring && NiagaraEffect)
	{
		PerformRaycast();
	}
}

void ALaserCannon::ActivateBeam(const bool bActive)
{
	if (NiagaraEffect)
	{
		bIsFiring = bActive;
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
	
    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_GameTraceChannel1,
        QueryParams
    );

    const FVector BeamEnd = bHit ? Hit.ImpactPoint : End;

    if (bHit)
    {
        AActor* OtherActor = Hit.GetActor();
        UPrimitiveComponent* ImpactComponent = Hit.GetComponent();
        AHcCharacterBase* ImpactCharacter = Cast<AHcCharacterBase>(OtherActor);

        if (OtherActor && OtherActor != this && ImpactComponent)
        {
            constexpr float LaserForce = 12500.0f;
            constexpr float LaserDamage = 1.0f;

            float CosTheta = FVector::DotProduct(ForwardVector, Hit.ImpactNormal);
            float AngleDegrees = FMath::RadiansToDegrees(FMath::Acos(CosTheta));
            UE_LOG(LogTemp, Log, TEXT("Hit Angle: %f degrees, CosTheta: %f"), AngleDegrees, CosTheta);

            if (ImpactCharacter)
            {
                ImpactCharacter->Damage(LaserDamage);
            }

            if (ImpactComponent->IsSimulatingPhysics())
            {
                FVector Impulse = ForwardVector * LaserForce * FMath::Abs(CosTheta);
                ImpactComponent->AddImpulseAtLocation(Impulse, Hit.ImpactPoint);
            }
            else if (ImpactCharacter && ImpactCharacter->IsDead())
            {
                FVector Impulse = ForwardVector * LaserForce * FMath::Abs(CosTheta);
                ImpactCharacter->GetMesh()->AddImpulseAtLocation(Impulse, Hit.ImpactPoint);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No hit detected! Check cube collision settings."));
    }

    NiagaraEffect->SetVectorParameter(TEXT("Beam End"), BeamEnd);
}
