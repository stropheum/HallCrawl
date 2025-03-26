#include "HcPlayerState.h"
#include "AttributeSetBase.h"
#include "HcAbilitySystemComponent.h"


AHcPlayerState::AHcPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UHcAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSetBase = CreateDefaultSubobject<UAttributeSetBase>(TEXT("HealthAttributeSet"));
}

UAttributeSetBase* AHcPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}

UAbilitySystemComponent* AHcPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
