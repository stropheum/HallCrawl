#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "AttributeSetBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewHealth, float, Delta);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShieldChanged, float, NewShield, float, Delta);

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class HALLCRAWL_API UAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:

	void ApplyDamage(const FGameplayEffectModCallbackData& Data);
	
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnShieldChanged OnShieldChanged;

	UPROPERTY(BlueprintReadOnly, Category="Attributes", meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, Health)

	UPROPERTY(BlueprintReadOnly, Category="Attributes", meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, Shield)

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
};
