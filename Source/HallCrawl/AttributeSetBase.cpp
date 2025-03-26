#include "AttributeSetBase.h"
#include "GameplayEffectExtension.h"


void UAttributeSetBase::ApplyDamage(const FGameplayEffectModCallbackData& Data)
{
	float Damage = Data.EvaluatedData.Magnitude;
	float HealthDamageApplied = 0.0f;
	float ShieldDamageApplied;
	if (Damage > Shield.GetCurrentValue())
	{
		HealthDamageApplied = Damage - Shield.GetCurrentValue();
		ShieldDamageApplied = Shield.GetCurrentValue();
		SetHealth(FMath::Max(Health.GetCurrentValue() - HealthDamageApplied, 0.0f));
		Damage -= Shield.GetCurrentValue();
		SetShield(0.0f);
	}
	else
	{
		ShieldDamageApplied = Damage;
	}
	
	SetShield(FMath::Max(Shield.GetCurrentValue() - Damage, 0.0f));

	OnHealthChanged.Broadcast(Health.GetCurrentValue(), -HealthDamageApplied);
	OnShieldChanged.Broadcast(Shield.GetCurrentValue(), -ShieldDamageApplied);
}

void UAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UAttributeSetBase::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}
