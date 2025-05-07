#pragma once
#include "GameplayAbilitySpecHandle.h"
#include "Components/SkeletalMeshComponent.h"
#include "HallCrawlWeaponComponent.generated.h"

class UGameplayAbility;
class AHallCrawlCharacter;
class ALaserCannon;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
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

    UFUNCTION(BlueprintCallable, Category="Weapon")
    void Fire(const bool IsTriggered) const;

    UFUNCTION(BlueprintCallable, Category="Weapon")
    void FireTriggered();

    UFUNCTION(BlueprintCallable, Category="Weapon")
    void FireOngoing();

    UFUNCTION(BlueprintCallable, Category="Weapon")
    void StopFiring();

    /** Check if this is a beam weapon */
    UFUNCTION(BlueprintCallable, Category="Weapon")
    bool IsBeamWeapon() const { return bIsBeamWeapon; }

    /** Get the LaserCannon actor for beam weapons */
    UFUNCTION(BlueprintCallable, Category="Weapon")
    ALaserCannon* GetLaserCannon() const { return LaserCannon; }

protected:
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PickupWeapon, meta = (AllowPrivateAccess = "true"))
    UMaterial* RenderTargetMaterial = nullptr;

    UPROPERTY()
    FGameplayAbilitySpecHandle FireAbilityHandle;

    UPROPERTY()
    UFireRifleAbility* FireAbility = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    bool bIsBeamWeapon = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<ALaserCannon> LaserCannonClass;

    UPROPERTY()
    ALaserCannon* LaserCannon = nullptr;

private:
    /** The Character holding this weapon */
    UPROPERTY()
    AHallCrawlCharacter* Character = nullptr;
};