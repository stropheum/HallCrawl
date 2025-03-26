#include "AttributeSetBase.h"
#include "GameplayEffectExtension.h"


UAttributeSetBase::UAttributeSetBase()
{
	Health = 100.0f;
	MaxHealth = 100.0f;
	Shield = 100.0f;
	MaxShield = 100.0f;
	ShieldRegenDelay = 5.0f;
	ShieldRegenRate = 20.0f;
}

void UAttributeSetBase::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetShieldAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxShield());
	}
}

void UAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		const float NewHealth = GetHealth();
		const float Delta = NewHealth - Data.EvaluatedData.Magnitude;
		OnHealthChanged.Broadcast(NewHealth, Delta);
	}
	else if (Data.EvaluatedData.Attribute == GetShieldAttribute())
	{
		const float NewShield = GetShield();
		const float Delta = NewShield - Data.EvaluatedData.Magnitude;
		OnHealthChanged.Broadcast(NewShield, Delta);
	}
}

void UAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UAttributeSetBase::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}
