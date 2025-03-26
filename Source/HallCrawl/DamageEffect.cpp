#include "DamageEffect.h"
#include "DamageExecutionCalculation.h"

UDamageEffect::UDamageEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayEffectExecutionDefinition ExecutionDef;
	ExecutionDef.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(ExecutionDef);
}