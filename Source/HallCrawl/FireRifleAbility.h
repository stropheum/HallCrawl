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
	UFireRifleAbility();

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

	UFUNCTION(BlueprintCallable, Category = "FireRifleAbility", meta = (AllowPrivateAccess = "true"))
	void FireProjectile(const FGameplayAbilitySpecHandle Handle);

	const FName TriggerTagName = FName("Weapon.Trigger.Triggered");
	const FName OngoingTagName = FName("Weapon.Trigger.Ongoing");

protected:
	UFUNCTION(BlueprintCallable, Category = "FireRifleAbility", meta = (AllowPrivateAccess = "true"))
	void SpawnBullets();

	UPROPERTY()
	class AHallCrawlCharacter* Character = nullptr;

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
