#include "FireRifleAbility.h"
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
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}

void UFireRifleAbility::ActivateAbility(
	const FSpecHandle Handle, const FActorInfo* ActorInfo, const FActivationInfo ActivationInfo, const FEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	AActor* Actor = GetAvatarActorFromActorInfo();
	check(Actor);
	const auto Character = Cast<AHallCrawlCharacter>(Actor);
	check(Character);
	
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	if (ProjectileClass == nullptr)
	{
		ProjectileClass = AHallCrawlProjectile::StaticClass();
		UE_LOG(LogGameplayTags, Warning, TEXT("ProjectileClass == nulllptr. Falling back to base class"));
	}
	check(ProjectileClass);
	
	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = Character->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
	
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	
			// Spawn the projectile at the muzzle
			World->SpawnActor<AHallCrawlProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
