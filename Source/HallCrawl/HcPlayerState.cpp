#include "AttributeSetBase.h"
#include "HcAbilitySystemComponent.h"
#include "HcPlayerState.h"


AHcPlayerState::AHcPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UHcAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSetBase = CreateDefaultSubobject<UAttributeSetBase>(TEXT("HealthAttributeSet"));
	DeadTag = FGameplayTag::RequestGameplayTag("State.Dead");
}

UAttributeSetBase* AHcPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}

UAbilitySystemComponent* AHcPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
