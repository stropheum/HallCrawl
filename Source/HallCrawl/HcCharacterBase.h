#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "AttributeSetBase.h"
#include "DamageEffect.h"
#include "ShieldRegenEffect.h"
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

	UFUNCTION()
	bool IsDead() const;

protected:
	
	virtual void BeginPlay() override;

	void OnHealthChangeCallback(const FOnAttributeChangeData& Data);
	
	void OnShieldChangeCallback(const FOnAttributeChangeData& Data) const;

	void TriggerRagdoll();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(BlueprintReadOnly, Category=HcCharacter, meta=(AllowPrivateAccess=true))
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=HcCharacter, meta=(AllowPrivateAccess=true))
	UAttributeSetBase* HealthAttributeSet = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category=HcCharacter, meta=(AllowPrivateAccess=true))
	TSubclassOf<UGameplayEffect> DamageEffectClass = UDamageEffect::StaticClass();
	
	UPROPERTY(BlueprintReadOnly, Category=HcCharacter, meta=(AllowPrivateAccess=true))
	TSubclassOf<UGameplayEffect> ShieldRegenEffectClass = UShieldRegenEffect::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=HcCharacter, meta=(AllowPrivateAccess=true))
	TObjectPtr<class UHealthBarWidget> HealthBarWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=HcCharacter, meta=(AllowPrivateAccess=true))
	TObjectPtr<class UWidgetComponent> HealthBarWidgetComponent = nullptr;
	
	FActiveGameplayEffectHandle ShieldRegenHandle;
	float TimeSinceLastDamage = 0.0f;
	bool bIsRegenerating = false;
	bool bIsDead = false;

private:
	void BillboardHealthBarToPlayer() const;

public:

	UFUNCTION()
	void Damage(float Damage) const;
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
