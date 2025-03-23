#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquippedWeaponWidget.generated.h"

class UImage;

UCLASS()
class HALLCRAWL_API UEquippedWeaponWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=EquippedWeapon, meta=(AllowPrivateAccess=true))
	UImage* WeaponImage = nullptr;

public:
	UFUNCTION()
	void SetWeaponImageMaterial(UMaterial* NewMaterial) const;
};
