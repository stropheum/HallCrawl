#include "ShieldRegenEffect.h"
#include "AttributeSetBase.h"

UShieldRegenEffect::UShieldRegenEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	Period = 1.0f;

	FGameplayModifierInfo Modifier;
	Modifier.Attribute = UAttributeSetBase::GetShieldAttribute();
	Modifier.ModifierOp = EGameplayModOp::Additive;
	Modifier.ModifierMagnitude = FScalableFloat(20.0f);
	Modifiers.Add(Modifier);

	StackingType = EGameplayEffectStackingType::None;
}