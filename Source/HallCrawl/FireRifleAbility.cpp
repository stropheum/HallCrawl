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


void UFireRifleAbility::ActivateAbility(
	const FSpecHandle Handle, const FActorInfo* ActorInfo, const FActivationInfo ActivationInfo, const FEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	AActor* Actor = GetAvatarActorFromActorInfo();
	check(Actor);
	Character = Cast<AHallCrawlCharacter>(Actor);
	check(Character);
	
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}
	
	switch (FireMode)
	{
	case EFireMode::Single:
		HandleSingleFire();
		break;
	case EFireMode::Auto:
		HandleAutoFire();
		break;
	case EFireMode::Burst:
		HandleBurstFire();
		break;
	}
}

void UFireRifleAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UFireRifleAbility::FireProjectile()
{
	if (!IsActive()) { return; }
	
	if (ProjectileClass == nullptr)
	{
		ProjectileClass = AHallCrawlProjectile::StaticClass();
		UE_LOG(LogGameplayTags, Warning, TEXT("ProjectileClass == nulllptr. Falling back to base class"));
	}
	check(ProjectileClass);
	
	UWorld* const World = GetWorld();
	if (!World) { return; }
		
	const APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	const FVector MuzzleOffset = Character->GetMuzzleOffset();
	const FVector SpawnLocation = Character->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
	
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	World->SpawnActor<AHallCrawlProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	
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
}

void UFireRifleAbility::HandleSingleFire()
{
	FireProjectile();
}

void UFireRifleAbility::HandleAutoFire()
{
	FireTask = UAbilityTask_WaitDelay::WaitDelay(this, FireRate);
	FireTask->OnFinish.AddDynamic(this, &UFireRifleAbility::FireProjectile);
	FireProjectile();
	FireTask->ReadyForActivation();
	FireTask->SetWaitingOnAvatar();
}

void UFireRifleAbility::HandleBurstFire()
{
	// TODO: implement burst fire
}