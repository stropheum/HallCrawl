#include "LaserCannon.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"


ALaserCannon::ALaserCannon()
{
	NiagaraEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraEffect"));
	RootComponent = NiagaraEffect;
	PrimaryActorTick.bCanEverTick = true;
}

void ALaserCannon::BeginPlay()
{
	Super::BeginPlay();

	RandomStream = FRandomStream(0);

	PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController not found!"));
		return;
	}

	if (Owner)
	{
		Owner->EnableInput(PlayerController);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Owner actor not found!"));
		return;
	}

	if (InputComponent)
	{
		InputComponent->BindAction("Fire", IE_Pressed, this, &ALaserCannon::HandleMouseClick);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InputComponent not found on owner!"));
		return;
	}

	InitializeNiagaraSystem();
}

void ALaserCannon::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	AlignToNearestActor(DeltaTime);
	PerformRaycast();
}

void ALaserCannon::AlignToNearestActor(const float DeltaTime)
{
	TimeSinceLastTargetSelection += DeltaTime;
	if (TimeSinceLastTargetSelection < 3.0f) { return; }

	TimeSinceLastTargetSelection = 0.0f;
	const float RandomAngle = RandomStream.FRandRange(0.f, 360.f);
	const FRotator Rotation(0.0f, RandomAngle, 0.0f);
	SetActorRotation(Rotation);
}

void ALaserCannon::PerformRaycast()
{
	const FVector Start = GetActorLocation();
	const FVector ForwardVector = GetActorForwardVector();
	constexpr float RayLength = 10000.0f;
	const FVector End = Start + (ForwardVector * RayLength);

	FHitResult Hit;
	const FCollisionQueryParams QueryParams;
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Visibility);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);

	const bool bHit = GetWorld()->LineTraceSingleByObjectType(
		Hit,
		Start,
		End,
		ObjectParams,
		QueryParams
	);

	if (const bool bIsActive = NiagaraEffect->IsActive(); !bIsActive && bHit)
	{
		NiagaraEffect->Activate(true);
	}
	else if (bIsActive && !bHit)
	{
		NiagaraEffect->Deactivate();
	}

	NiagaraEffect->SetVectorParameter("Beam End", Hit.ImpactPoint);
}

void ALaserCannon::InitializeNiagaraSystem()
{
	if (NiagaraEffect && NiagaraSystemAsset)
	{
		NiagaraEffect->SetAsset(NiagaraSystemAsset);
		NiagaraEffect->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		NiagaraEffect->Activate(true);
		NiagaraEffect->SetVectorParameter(TEXT("Beam_End"), GetOwner()->GetActorLocation());
		const FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NiagaraSystemAsset or NiagaraEffect is null! Ensure NiagaraSystemAsset is assigned in the Editor."));
	}
}

void ALaserCannon::HandleMouseClick()
{
	if (!NiagaraEffect || !PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("NiagaraEffect or PlayerController is null!"));
		return;
	}

	FVector HitLocation;
	if (GetMouseClickPosition(HitLocation))
	{
		NiagaraEffect->SetVectorParameter(TEXT("Beam_End"), HitLocation);
		DrawDebugSphere(GetWorld(), HitLocation, 10.0f, 12, FColor::Red, false, 2.0f);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("No valid hit location found for mouse click."));
	}
}

bool ALaserCannon::GetMouseClickPosition(FVector& OutHitLocation)
{
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController is null!"));
		return false;
	}

	if (FVector2D MousePosition; !PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get mouse position!"));
		return false;
	}

	FVector WorldDirection;
	if (FVector WorldOrigin; PlayerController->DeprojectMousePositionToWorld(WorldOrigin, WorldDirection))
	{
		const FVector TraceEnd = WorldOrigin + (WorldDirection * 10000.0f);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetOwner());

		if (FHitResult HitResult; GetWorld()->LineTraceSingleByChannel(HitResult, WorldOrigin, TraceEnd, ECC_Visibility, QueryParams))
		{
			OutHitLocation = HitResult.Location;
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Line trace did not hit any objects."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to deproject mouse position to world space."));
	}

	return false;
}
