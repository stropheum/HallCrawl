#include "HcCharacterBase.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HealthBarWidget.h"
#include "Components/WidgetComponent.h"


AHcCharacterBase::AHcCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	HealthAttributeSet = CreateDefaultSubobject<UAttributeSetBase>("HealthAttributeSet");
}

void AHcCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	check(AbilitySystemComponent);
	check(HealthAttributeSet);
	check(DamageEffectClass);
	check(ShieldRegenEffectClass);

	for (UActorComponent* Component : GetComponents())
	{
		if (UWidgetComponent* WidgetComponent = Cast<UWidgetComponent>(Component))
		{
			if (UHealthBarWidget* FoundWidget = Cast<UHealthBarWidget>(WidgetComponent->GetUserWidgetObject()))
			{
				HealthBarWidgetComponent = WidgetComponent;
				HealthBarWidget = FoundWidget;
				break;
			}
		}
	}
	check(HealthBarWidget);

	AbilitySystemComponent->AddAttributeSetSubobject(HealthAttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthAttributeSet->GetHealthAttribute())
		.AddUObject(this, &AHcCharacterBase::OnHealthChangeCallback);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthAttributeSet->GetShieldAttribute())
		.AddUObject(this, &AHcCharacterBase::OnShieldChangeCallback);

	HealthBarWidget->UpdateValues(HealthAttributeSet);
}

void AHcCharacterBase::OnHealthChangeCallback(const FOnAttributeChangeData& Data)
{
	GEngine->AddOnScreenDebugMessage(
		-1, 5.f, FColor::Red, FString::Printf(TEXT("%s Health: %f"), *GetName(), Data.NewValue));
	HealthBarWidget->UpdateValues(HealthAttributeSet);

	if (Data.NewValue <= 0.0f)
	{
		HealthBarWidget->SetVisibility(ESlateVisibility::Hidden);
		TriggerRagdoll();
	}
}

void AHcCharacterBase::OnShieldChangeCallback(const FOnAttributeChangeData& Data) const
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue,
		FString::Printf(TEXT("%s Shield: %f"), *GetName(), Data.NewValue));
	HealthBarWidget->UpdateValues(HealthAttributeSet);

	if (Data.NewValue < Data.OldValue)
	{
		const_cast<AHcCharacterBase*>(this)->TimeSinceLastDamage = 0.0f;
		if (bIsRegenerating)
		{
			const_cast<AHcCharacterBase*>(this)->AbilitySystemComponent->RemoveActiveGameplayEffect(ShieldRegenHandle);
			const_cast<AHcCharacterBase*>(this)->bIsRegenerating = false;
		}
	}
}

void AHcCharacterBase::TriggerRagdoll()
{
	if (USkeletalMeshComponent* SkeletalMesh = GetMesh())
	{
		UCapsuleComponent* Capsule = GetCapsuleComponent();
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Capsule->SetSimulatePhysics(false);
		
		GetCharacterMovement()->DisableMovement();
		SkeletalMesh->SetSimulatePhysics(true);
		SkeletalMesh->WakeAllRigidBodies();
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SkeletalMesh->SetEnableGravity(true);
		SkeletalMesh->SetLinearDamping(0.5f);
		SkeletalMesh->SetAngularDamping(0.5f);
		SkeletalMesh->SetAllBodiesBelowSimulatePhysics("root", true);

		DisableInput(nullptr);
		bIsDead = true;
		SetLifeSpan(30.0f);
	}
}

UAbilitySystemComponent* AHcCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool AHcCharacterBase::IsDead() const
{
	return bIsDead;
}

void AHcCharacterBase::BillboardHealthBarToPlayer() const
{
	if (HealthBarWidget)
	{
		if (const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			FVector CameraLocation;
			FRotator CameraRotation;
			PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
			const FVector WidgetLocation = HealthBarWidgetComponent->GetComponentLocation();
			const FRotator LookAtRotation = FRotationMatrix::MakeFromX(CameraLocation - WidgetLocation).Rotator();
			HealthBarWidgetComponent->SetWorldRotation(FRotator(0.0f, LookAtRotation.Yaw, 0.0f));
		}
	}
}

void AHcCharacterBase::Damage(const float Damage) const
{
	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
		DamageEffectClass, 1.0f, AbilitySystemComponent->MakeEffectContext());

	if (SpecHandle.IsValid())
	{
		const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(UAttributeSetBase::GetDamageTagName());
		SpecHandle.Data->SetSetByCallerMagnitude(Tag, Damage);
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AHcCharacterBase::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (HealthAttributeSet->GetShield() < HealthAttributeSet->GetMaxShield())
	{
		TimeSinceLastDamage += DeltaTime;

		if (TimeSinceLastDamage >= HealthAttributeSet->GetShieldRegenDelay() && !bIsRegenerating)
		{
			const FGameplayEffectSpecHandle RegenSpec = AbilitySystemComponent->MakeOutgoingSpec(
				ShieldRegenEffectClass, 1.0f, AbilitySystemComponent->MakeEffectContext());
			if (RegenSpec.IsValid())
			{
				ShieldRegenHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*RegenSpec.Data.Get());
				bIsRegenerating = true;
			}
		}
	}
	else if (bIsRegenerating && HealthAttributeSet->GetShield() >= HealthAttributeSet->GetMaxShield())
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(ShieldRegenHandle);
		bIsRegenerating = false;
	}

	BillboardHealthBarToPlayer();
}

void AHcCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
