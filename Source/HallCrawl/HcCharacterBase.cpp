#include "HcCharacterBase.h"


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

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthAttributeSet->GetHealthAttribute()).AddUObject(
		this, &AHcCharacterBase::OnHealthChangeCallback);
}

void AHcCharacterBase::OnHealthChangeCallback(const FOnAttributeChangeData& Data) const
{
	GEngine->AddOnScreenDebugMessage(
		-1, 5.f, FColor::Red, FString::Printf(TEXT("%s Health: %f"), *GetName(), Data.NewValue));
}

void AHcCharacterBase::OnShieldChangeCallback(const FOnAttributeChangeData& Data) const
{
	GEngine->AddOnScreenDebugMessage(
		-1, 5.f, FColor::Red, FString::Printf(TEXT("%s Shield: %f"), *GetName(), Data.NewValue));
}

UAbilitySystemComponent* AHcCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AHcCharacterBase::Damage(const float Damage) const
{
	// HealthAttributeSet->ApplyDamage(Damage);
	//TODO: replace with adding effect
}

void AHcCharacterBase::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AHcCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
