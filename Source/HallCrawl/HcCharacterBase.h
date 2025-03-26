﻿#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "AttributeSetBase.h"
#include "GameFramework/Character.h"
#include "HcCharacterBase.generated.h"


class UAttributeSetBase;
struct FOnAttributeChangeData;

UCLASS()
class HALLCRAWL_API AHcCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	
	AHcCharacterBase();

protected:
	
	virtual void BeginPlay() override;

	void OnHealthChangeCallback(const FOnAttributeChangeData& Data) const;
	
	void OnShieldChangeCallback(const FOnAttributeChangeData& Data) const;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(BlueprintReadOnly, Category=HcCharacter, meta=(AllowPrivateAccess=true))
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=HcCharacter, meta=(AllowPrivateAccess=true))
	UAttributeSetBase* HealthAttributeSet = nullptr;

public:

	UFUNCTION()
	void Damage(float Damage) const;
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
