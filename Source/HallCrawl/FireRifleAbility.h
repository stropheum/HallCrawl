#pragma once

#include "HallCrawlPCH.h"
#include "FireRifleAbility.generated.h"

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

	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AHallCrawlProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;
};
