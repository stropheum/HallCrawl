// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "HallCrawlPCH.h"
#include "Components/SkeletalMeshComponent.h"
#include "HallCrawlWeaponComponent.generated.h"

class AHallCrawlCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HALLCRAWL_API UHallCrawlWeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<class UFireRifleAbility> FireRifleAbilityClass;
	
	// /** Projectile class to spawn */
	// UPROPERTY(EditDefaultsOnly, Category=Projectile)
	// TSubclassOf<class AHallCrawlProjectile> ProjectileClass;
	//
	// /** Sound to play each time we fire */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	// USoundBase* FireSound;
	//
	// /** AnimMontage to play each time we fire */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	// UAnimMontage* FireAnimation;
	//
	// /** Gun muzzle's offset from the characters location */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	// FVector MuzzleOffset;
	//
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

protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	FGameplayAbilitySpecHandle FireAbilityHandle;

private:
	/** The Character holding this weapon*/
	UPROPERTY()
	AHallCrawlCharacter* Character = nullptr;
};
