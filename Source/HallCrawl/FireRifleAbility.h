#pragma once

#include "Abilities/GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "FireRifleAbility.generated.h"

UENUM()
enum class EFireMode : uint8 { Single, Auto, Burst };

UCLASS()
class HALLCRAWL_API UFireRifleAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;
	
protected:
	UFUNCTION()
	void FireProjectile();

	UFUNCTION()
	void HandleSingleFire();

	UFUNCTION()
	void HandleAutoFire();
	
	UFUNCTION()
	void HandleBurstFire();

	UPROPERTY()
	class AHallCrawlCharacter* Character;
	
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Projectile)
	TSubclassOf<class AHallCrawlProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	EFireMode FireMode = EFireMode::Single;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float FireRate = 0.15f;

protected:
	UPROPERTY()
	UAbilityTask_WaitDelay* FireTask = nullptr;
};
