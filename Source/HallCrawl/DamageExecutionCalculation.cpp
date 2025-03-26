#include "DamageExecutionCalculation.h"
#include "GameplayAbilities/Public/AbilitySystemGlobals.h"

#include "AttributeSetBase.h"

UDamageExecutionCalculation::UDamageExecutionCalculation()
{
	FGameplayEffectAttributeCaptureDefinition ShieldDef;
	ShieldDef.AttributeToCapture = UAttributeSetBase::GetShieldAttribute();
	ShieldDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	ShieldDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(ShieldDef);

	FGameplayEffectAttributeCaptureDefinition HealthDef;
	HealthDef.AttributeToCapture = UAttributeSetBase::GetHealthAttribute();
	HealthDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	HealthDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(HealthDef);
}

void UDamageExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                         FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{

    const AActor* TargetActor = ExecutionParams.GetTargetAbilitySystemComponent()->GetAvatarActor();
    const UAbilitySystemComponent* TargetAbilityComp = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);

    if (const UAttributeSetBase* AttributeSet =
        TargetAbilityComp ? Cast<UAttributeSetBase>(TargetAbilityComp->GetAttributeSet(UAttributeSetBase::StaticClass())) : nullptr;
        !TargetAbilityComp || !AttributeSet)
    {
        return;
    }
    
    float CurrentShield = 0.0f;
    FGameplayEffectAttributeCaptureDefinition ShieldCaptureDefinition(
        UAttributeSetBase::GetShieldAttribute(), EGameplayEffectAttributeCaptureSource::Target, true);
    
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        ShieldCaptureDefinition, FAggregatorEvaluateParameters(), CurrentShield);
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        ShieldCaptureDefinition, FAggregatorEvaluateParameters(), CurrentShield);

    float CurrentHealth = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ShieldCaptureDefinition, FAggregatorEvaluateParameters(), CurrentHealth);

    const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();
    const float DamageAmount = EffectSpec.GetSetByCallerMagnitude(
        FGameplayTag::RequestGameplayTag(UAttributeSetBase::GetDamageTagName()), false, 0.0f);

    if (DamageAmount > 0.0f)
    {
        float ShieldDamage = 0.0f;
        float HealthDamage = 0.0f;
        float RemainingDamage = DamageAmount;
        
        if (CurrentShield > 0.0f)
        {
            ShieldDamage = FMath::Min(RemainingDamage, CurrentShield);
            RemainingDamage -= ShieldDamage;
        }
        
        if (RemainingDamage > 0.0f)
        {
            HealthDamage = RemainingDamage;
        }

        if (ShieldDamage > 0.0f)
        {
            OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
                UAttributeSetBase::GetShieldAttribute(),
                EGameplayModOp::Additive,
                -ShieldDamage
            ));
        }
        if (HealthDamage > 0.0f)
        {
            OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
                UAttributeSetBase::GetHealthAttribute(),
                EGameplayModOp::Additive,
                -HealthDamage
            ));
        }
    }
}
