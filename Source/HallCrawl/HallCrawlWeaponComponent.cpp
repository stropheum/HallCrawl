#include "HallCrawlWeaponComponent.h"

#include "HallCrawlCharacter.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FireRifleAbility.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "AbilitySystemComponent.h"


void UHallCrawlWeaponComponent::Fire()
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
	
	if (!Character->GetAbilitySystemComponent()->TryActivateAbility(FireAbilityHandle))
	{
		UE_LOG(LogTemp, Warning, TEXT("Fire weapon attempt(by handle) failed"));
		if (FireRifleAbilityClass && !Character->GetAbilitySystemComponent()->TryActivateAbilityByClass(FireRifleAbilityClass))
		{
			UE_LOG(LogTemp, Warning, TEXT("Fire weapon attempt(by class) failed"));
		}
	}
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
	if (FireAbilityHandle.IsValid() && FireAbility->FireMode == EFireMode::Auto && FireAbility->IsActive())
	{
		HandleAutoFire();
	}
}

void UHallCrawlWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UHallCrawlWeaponComponent::AttachWeapon(AHallCrawlCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	// Check that the character is valid, and has no weapon component yet
	if (Character == nullptr || Character->GetInstanceComponents().FindItemByClass<UHallCrawlWeaponComponent>())
	{
		return false;
	}
	
	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	// Grant the FireRifleAbility to the character's AbilitySystemComponent
	if (FireRifleAbilityClass && Character->GetAbilitySystemComponent())
	{
		const FGameplayAbilitySpec AbilitySpec(FireRifleAbilityClass, 1, -1, this);
		FireAbilityHandle = Character->GetAbilitySystemComponent()->GiveAbility(AbilitySpec);
		FireAbility = Cast<UFireRifleAbility>(AbilitySpec.Ability);
		check(FireAbility);
	}

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UHallCrawlWeaponComponent::Fire);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &UHallCrawlWeaponComponent::StopFiring);
		}
	}

	return true;
}

void UHallCrawlWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// ensure we have a character owner
	if (Character != nullptr)
	{
		// remove the input mapping context from the Player Controller
		if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
				PlayerController->GetLocalPlayer()))
			{
				Subsystem->RemoveMappingContext(FireMappingContext);
			}
		}
	}

	// maintain the EndPlay call chain
	Super::EndPlay(EndPlayReason);
}

void UHallCrawlWeaponComponent::HandleSingleFire() const
{
	
}

void UHallCrawlWeaponComponent::HandleAutoFire() {}

void UHallCrawlWeaponComponent::HandleBurstFire() {}
