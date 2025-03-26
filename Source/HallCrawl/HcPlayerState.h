#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "GameplayTagContainer.h"
#include "HcPlayerState.generated.h"

UCLASS()
class HALLCRAWL_API AHcPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AHcPlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	class UAttributeSetBase* GetAttributeSetBase() const;

protected:

	UPROPERTY()
	FGameplayTag DeadTag;

	UPROPERTY()
	class UHcAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UAttributeSetBase* AttributeSetBase;
};