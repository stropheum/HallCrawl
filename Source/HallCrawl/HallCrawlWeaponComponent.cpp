#include "HallCrawlWeaponComponent.h"
#include "HallCrawlCharacter.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FireRifleAbility.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "AbilitySystemComponent.h"
#include "HallCrawlPlayerController.h"
#include "LaserCannon.h"


void UHallCrawlWeaponComponent::Fire(const bool IsTriggered) const
{
	if (!Character || !Character->GetController())
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted to fire with improper conditions"));
		return;
	}

	if (!FireAbilityHandle.IsValid() || !FireAbility)
	{
		UE_LOG(LogTemp, Warning, TEXT("No FireRifleAbility set"));
		return;
	}

	UAbilitySystemComponent* Asc = Character->GetAbilitySystemComponent();
	FGameplayAbilitySpec* AbilitySpec = Asc->FindAbilitySpecFromHandle(FireAbilityHandle);
	ensureMsgf(AbilitySpec, TEXT("Ability spec not found for fire ability handle"));

	const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(IsTriggered
		                                                          ? FName("Weapon.Trigger.Triggered")
		                                                          : FName("Weapon.Trigger.Ongoing"));

	AbilitySpec->GetDynamicSpecSourceTags().AddTag(Tag);
	Asc->TryActivateAbility(FireAbilityHandle);
}


void UHallCrawlWeaponComponent::FireTriggered()
{
	Fire(true);
}

void UHallCrawlWeaponComponent::FireOngoing()
{
	Fire(false);
}

void UHallCrawlWeaponComponent::StopFiring()
{
	if (!Character || !Character->GetController())
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted to fire with improper conditions"));
		return;
	}

	if (!FireAbilityHandle.IsValid() || !FireAbility)
	{
		UE_LOG(LogTemp, Warning, TEXT("No FireRifleAbility set"));
		return;
	}

	Character->GetAbilitySystemComponent()->CancelAbilityHandle(FireAbilityHandle);
}

void UHallCrawlWeaponComponent::TickComponent(
	const float DeltaTime,
	const ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHallCrawlWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	ensureMsgf(RenderTargetMaterial, TEXT("RenderTargetMaterial != nullptr"));

	if (bIsBeamWeapon && LaserCannonClass)
	{
		if (UWorld* World = GetWorld())
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();

			const FVector RightVector = GetRightVector();
			const FRotator SpawnRotation = RightVector.Rotation();
			
			LaserCannon = World->SpawnActor<ALaserCannon>(LaserCannonClass, FVector::Zero(), SpawnRotation, SpawnParams);
			if (LaserCannon)
			{
				const FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
				LaserCannon->AttachToComponent(this, AttachmentRules, TEXT("MuzzleSocket"));
				LaserCannon->SetActorRelativeLocation(FVector(0.0f, 75.0f, 10.0f));
			}
		}
	}
}

bool UHallCrawlWeaponComponent::AttachWeapon(AHallCrawlCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	if (Character == nullptr || Character->GetInstanceComponents().FindItemByClass<UHallCrawlWeaponComponent>())
	{
		return false;
	}

	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	USkeletalMeshComponent* Mesh1P = Character->GetMesh1P();
	const FName SocketName = FName(TEXT("GripPoint"));
	TArray<USceneComponent*> AttachedComponents;
	Mesh1P->GetChildrenComponents(false, AttachedComponents);

	for (USceneComponent* Component : AttachedComponents)
	{
		if (Component && Component->GetAttachSocketName() == SocketName)
		{
			Component->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			Component->DestroyComponent();
		}
	}

	AttachToComponent(Mesh1P, AttachmentRules, SocketName);
	Character->EquippedWeapon = this;

	if (FireRifleAbilityClass && Character->GetAbilitySystemComponent())
	{
		const FGameplayAbilitySpec AbilitySpec(FireRifleAbilityClass, 1, -1, this);
		FireAbilityHandle = Character->GetAbilitySystemComponent()->GiveAbility(AbilitySpec);
		FireAbility = Cast<UFireRifleAbility>(AbilitySpec.Ability);
		check(FireAbility);
	}

	if (AHallCrawlPlayerController* PlayerController = Cast<AHallCrawlPlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &UHallCrawlWeaponComponent::FireTriggered);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Ongoing, this, &UHallCrawlWeaponComponent::FireOngoing);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &UHallCrawlWeaponComponent::StopFiring);
		}

		ensureMsgf(RenderTargetMaterial, TEXT("RenderTargetMaterial != nullptr"));
		PlayerController->SetEquippedWeaponHudMaterial(RenderTargetMaterial);
	}

	return true;
}

void UHallCrawlWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopFiring();
	if (Character != nullptr)
	{
		const UAbilitySystemComponent* Asc = Character->GetAbilitySystemComponent();
		FGameplayAbilitySpec* AbilitySpec = Asc->FindAbilitySpecFromHandle(FireAbilityHandle);
		ensureMsgf(AbilitySpec, TEXT("Ability spec not found for fire ability handle"));

		FGameplayTagContainer TagsToRemove;
		TagsToRemove.AddTag(FGameplayTag::RequestGameplayTag(FName("Weapon.Trigger.Triggered")));
		TagsToRemove.AddTag(FGameplayTag::RequestGameplayTag(FName("Weapon.Trigger.Ongoing")));

		AbilitySpec->GetDynamicSpecSourceTags().RemoveTags(TagsToRemove);

		if (const APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
				PlayerController->GetLocalPlayer()))
			{
				Subsystem->RemoveMappingContext(FireMappingContext);
			}
		}
	}

	if (LaserCannon)
	{
		LaserCannon->Destroy();
		LaserCannon = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}
