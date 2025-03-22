#include "EquippedWeaponWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"


void UEquippedWeaponWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UEquippedWeaponWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UEquippedWeaponWidget::SetWeaponImageMaterial(UMaterial* NewMaterial) const
{
	if (!ensureMsgf(WeaponImage, TEXT("WeaponImage != nullptr"))) { return; }
	WeaponImage->SetBrushFromMaterial(NewMaterial);
}
