#include "FireRifleAbility.h"

#include "AbilitySystemComponent.h"
#include "HallCrawlCharacter.h"
#include "HallCrawlProjectile.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Text/ISlateEditableTextWidget.h"


void UFireRifleAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
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
	FireProjectile(Handle);
}

void UFireRifleAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (!Character) { return; }
	UAbilitySystemComponent* Asc = Character->GetAbilitySystemComponent();
	FGameplayAbilitySpec* AbilitySpec = Asc->FindAbilitySpecFromHandle(Handle);

	const FGameplayTag TriggerTag = FGameplayTag::RequestGameplayTag(TriggerTagName);
	const FGameplayTag OngoingTag = FGameplayTag::RequestGameplayTag(OngoingTagName);

	AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(TriggerTag);
	AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(OngoingTag);
}

void UFireRifleAbility::FireProjectile(const FGameplayAbilitySpecHandle Handle)
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
	if (Character == nullptr || Character->GetController() == nullptr) { return; }

	const FGameplayTag OngoingTag = FGameplayTag::RequestGameplayTag(OngoingTagName);
	const UAbilitySystemComponent* Asc = Character->GetAbilitySystemComponent();
	FGameplayAbilitySpec* AbilitySpec = Asc->FindAbilitySpecFromHandle(Handle);
	if (!ensureMsgf(AbilitySpec, TEXT("AbilitySpec is null"))) { return; }

	if (FireMode != EFireMode::Auto && AbilitySpec->GetDynamicSpecSourceTags().HasTag(OngoingTag)) { return; }

	if (ProjectileClass == nullptr)
	{
		ProjectileClass = AHallCrawlProjectile::StaticClass();
		UE_LOG(LogGameplayTags, Warning, TEXT("ProjectileClass == nulllptr. Falling back to base class"));
	}
	check(ProjectileClass);

	SpawnBullets(*AbilitySpec);

	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}

	if (FireAnimation != nullptr)
	{
		if (UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance(); AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UFireRifleAbility::SpawnBullets(const FGameplayAbilitySpec AbilitySpec)
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World == nullptr. Aborting FireProjectile"));
		return;
	}

	const APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	const FRotator PlayerRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	const FVector MuzzleOffset = Character->GetMuzzleOffset();
	auto SpawnLocation = Character->GetActorLocation() + PlayerRotation.RotateVector(MuzzleOffset);

	if (AbilitySpec.GetDynamicSpecSourceTags().HasTag(FGameplayTag::RequestGameplayTag(OngoingTagName)))
	{
		FVector SpreadOffset = FVector(
			FMath::RandRange(-OriginSpread, OriginSpread),
			0.0f,
			FMath::RandRange(-OriginSpread, OriginSpread));
		SpawnLocation += PlayerRotation.RotateVector(SpreadOffset);
	}

	if (FireMode == EFireMode::Burst)
	{
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		auto RightOffset = SpawnLocation + PlayerRotation.RotateVector(FVector(0.0f, 50.0f, 0.0f));
		auto LeftOffset = SpawnLocation + PlayerRotation.RotateVector(FVector(0.0f, -50.0f, 0.0f));
		auto UpOffset = SpawnLocation + PlayerRotation.RotateVector(FVector(0.0f, 0.0f, 50.0f));
		auto DownOffset = SpawnLocation + PlayerRotation.RotateVector(FVector(0.0f, 0.0f, -50.0f));
		World->SpawnActor<AHallCrawlProjectile>(ProjectileClass, RightOffset, PlayerRotation, ActorSpawnParams);
		World->SpawnActor<AHallCrawlProjectile>(ProjectileClass, LeftOffset, PlayerRotation, ActorSpawnParams);
		World->SpawnActor<AHallCrawlProjectile>(ProjectileClass, UpOffset, PlayerRotation, ActorSpawnParams);
		World->SpawnActor<AHallCrawlProjectile>(ProjectileClass, DownOffset, PlayerRotation, ActorSpawnParams);
	}
	else
	{
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		World->SpawnActor<AHallCrawlProjectile>(ProjectileClass, SpawnLocation, PlayerRotation, ActorSpawnParams);
	}
}
