#include "FireRifleAbility.h"

#include "AbilitySystemComponent.h"
#include "HallCrawlCharacter.h"
#include "HallCrawlProjectile.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Text/ISlateEditableTextWidget.h"

using FSpecHandle = FGameplayAbilitySpecHandle;
using FActorInfo = FGameplayAbilityActorInfo;
using FActivationInfo = FGameplayAbilityActivationInfo;
using FEventData = FGameplayEventData;


UFireRifleAbility::UFireRifleAbility()
{
	FGameplayTag NewTag;
	switch (FireMode)
	{
	case EFireMode::Single:
		NewTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Rifle.Single"));
		break;
	case EFireMode::Auto:
		NewTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Rifle.Auto"));
		break;
	case EFireMode::Burst:
		NewTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Rifle.Burst"));
		break;
	}
	AbilityTags.AddTag(NewTag);
}

void UFireRifleAbility::ActivateAbility(
	const FSpecHandle Handle, const FActorInfo* ActorInfo, const FActivationInfo ActivationInfo, const FEventData* TriggerEventData)
{
	AActor* Actor = GetAvatarActorFromActorInfo();
	check(Actor);

	if (!Character)
	{
		Character = Cast<AHallCrawlCharacter>(Actor);		
	}
	check(Character);
	
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}
	
	ApplyCooldown(Handle, ActorInfo, ActivationInfo);
	FireProjectile();	
}

void UFireRifleAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (!Character) { return; }
	UAbilitySystemComponent* Asc = Character->GetAbilitySystemComponent();
	
	const FGameplayTag TriggerTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Trigger.Triggered"));
	const FGameplayTag OngoingTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Trigger.Ongoing"));
	Asc->RemoveLooseGameplayTag(TriggerTag);
	Asc->RemoveLooseGameplayTag(OngoingTag);
}

void UFireRifleAbility::FireProjectile()
{
	if (!IsActive())
	{
		UE_LOG(LogTemp, Error, TEXT("Inactive. Aborting"));
		return;
	}
	
	AActor* Actor = GetAvatarActorFromActorInfo();
	check(Actor);
	Character = Cast<AHallCrawlCharacter>(Actor);
	check(Character);
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}
	
	const FGameplayTag OngoingTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Trigger.Ongoing"));
	if (const UAbilitySystemComponent* Asc = Character->GetAbilitySystemComponent();
		FireMode == EFireMode::Single && Asc->HasMatchingGameplayTag(OngoingTag)) { return; }
	
	if (ProjectileClass == nullptr)
	{
		ProjectileClass = AHallCrawlProjectile::StaticClass();
		UE_LOG(LogGameplayTags, Warning, TEXT("ProjectileClass == nulllptr. Falling back to base class"));
	}
	check(ProjectileClass);
	
	SpawnBullets();
	
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	if (FireAnimation != nullptr)
	{
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
	
	CurrentVolleyBulletsFired++;
}

void UFireRifleAbility::Reset()
{
	CurrentVolleyBulletsFired = 0;
}

void UFireRifleAbility::FireProjectileOngoing()
{
	if (!IsActive() || (FireMode != EFireMode::Auto && CurrentVolleyBulletsFired >= 0))
	{
		return;
	}
	FireProjectile();
}

void UFireRifleAbility::SpawnBullets()
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World == nullptr. Aborting FireProjectile"));
		return;
	}
	
	const APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	const FVector MuzzleOffset = Character->GetMuzzleOffset();
	const FVector SpawnLocation = Character->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
	
	if (FireMode == EFireMode::Burst)
	{
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		auto RightOffset = SpawnLocation + FVector(100.0f, 0.0f, 0.0f);
		auto LeftOffset = SpawnLocation + FVector(-100.0f, 0.0f, 0.0f);
		auto UpOffset = SpawnLocation + FVector(0.0f, 100.0f, 0.0f);
		auto DownOffset = SpawnLocation + FVector(0.0f, -100.0f, 0.0f);
		World->SpawnActor<AHallCrawlProjectile>(ProjectileClass, RightOffset, SpawnRotation, ActorSpawnParams);
		World->SpawnActor<AHallCrawlProjectile>(ProjectileClass, LeftOffset, SpawnRotation, ActorSpawnParams);
		World->SpawnActor<AHallCrawlProjectile>(ProjectileClass, UpOffset, SpawnRotation, ActorSpawnParams);
		World->SpawnActor<AHallCrawlProjectile>(ProjectileClass, DownOffset, SpawnRotation, ActorSpawnParams);
	}
	else
	{
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		AHallCrawlProjectile* Bullet = World->SpawnActor<AHallCrawlProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);	
	}
}
