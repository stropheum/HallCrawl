#pragma once
#include "Abilities/GameplayAbility.h"
#include "FireRifleAbility.generated.h"

class AHallCrawlCharacter;
class ALaserCannon;
class UNiagaraComponent;

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	Single,
	Burst,
	Auto
};

UCLASS()
class UFireRifleAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	void FireProjectile(const FGameplayAbilitySpecHandle Handle);
	void SpawnBullets(const FGameplayAbilitySpec AbilitySpec);

	UPROPERTY()
	AHallCrawlCharacter* Character;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	EFireMode FireMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float OriginSpread;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	FString TriggerTagName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	FString OngoingTagName;

private:
	void ActivateBeamWeapon();
	void DeactivateBeamWeapon();

	UPROPERTY()
	ALaserCannon* LaserCannon;
};