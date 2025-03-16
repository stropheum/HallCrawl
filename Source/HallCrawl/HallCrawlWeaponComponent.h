// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayAbilitySpecHandle.h"
#include "Components/SkeletalMeshComponent.h"
#include "HallCrawlWeaponComponent.generated.h"

class UGameplayAbility;
class AHallCrawlCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HALLCRAWL_API UHallCrawlWeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<class UFireRifleAbility> FireRifleAbilityClass;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;
	
	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	bool AttachWeapon(AHallCrawlCharacter* TargetCharacter);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire();
	
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void StopFiring();

protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	/** Ends gameplay for this component. */

	UFUNCTION()
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void HandleSingleFire() const;
	void HandleAutoFire();
	void HandleBurstFire();
	
	UPROPERTY()
	FGameplayAbilitySpecHandle FireAbilityHandle;

	UPROPERTY()
	UFireRifleAbility* FireAbility = nullptr;

private:
	/** The Character holding this weapon*/
	UPROPERTY()
	AHallCrawlCharacter* Character = nullptr;
};
