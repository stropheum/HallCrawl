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


void UHallCrawlWeaponComponent::Fire(const bool IsTriggered)
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
	const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(IsTriggered
		? FName("Weapon.Trigger.Triggered")
		: FName("Weapon.Trigger.Ongoing"));
	Asc->AddLooseGameplayTag(Tag);
	
	if (!Asc->TryActivateAbility(FireAbilityHandle))
	{
		UE_LOG(LogTemp, Warning, TEXT("Fire weapon attempt(by handle) failed"));
	}
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
}

bool UHallCrawlWeaponComponent::AttachWeapon(AHallCrawlCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	// Check that the character is valid, and has no weapon component yet
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

	// Grant the FireRifleAbility to the character's AbilitySystemComponent
	if (FireRifleAbilityClass && Character->GetAbilitySystemComponent())
	{
		const FGameplayAbilitySpec AbilitySpec(FireRifleAbilityClass, 1, -1, this);
		FireAbilityHandle = Character->GetAbilitySystemComponent()->GiveAbility(AbilitySpec);
		FireAbility = Cast<UFireRifleAbility>(AbilitySpec.Ability);
		check(FireAbility);
	}

	// Set up action bindings
	if (AHallCrawlPlayerController* PlayerController = Cast<AHallCrawlPlayerController>(Character->GetController()))
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
	// ensure we have a character owner
	if (Character != nullptr)
	{
		UAbilitySystemComponent* Asc = Character->GetAbilitySystemComponent();
		FGameplayTagContainer LooseGameplayTags;
		LooseGameplayTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Weapon.Trigger.Triggered")));
		LooseGameplayTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Weapon.Trigger.Ongoing")));
		Asc->RemoveLooseGameplayTags(LooseGameplayTags);
		
		// remove the input mapping context from the Player Controller
		if (const APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
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
