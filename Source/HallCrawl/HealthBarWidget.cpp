#include "HealthBarWidget.h"
#include "AttributeSetBase.h"
#include "Components/ProgressBar.h"

void UHealthBarWidget::UpdateValues(const UAttributeSetBase* HealthAttributeSet) const
{
	if (HealthBar && HealthBar->GetIsEnabled())
	{
		HealthBar->SetPercent(HealthAttributeSet->GetHealth() / HealthAttributeSet->GetMaxHealth());
		if (HealthAttributeSet->GetHealth() <= 0.0f)
		{
			HealthBar->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	//TODO: implement shield damage properly
	if (ShieldBar && ShieldBar->GetIsEnabled())
	{
		ShieldBar->SetVisibility(ESlateVisibility::Collapsed);
		// ShieldBar->SetPercent(HealthAttributeSet->GetShield() / HealthAttributeSet->GetMaxShield());	
	}
}
