#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

class UProgressBar;
/**
 * A Simple health bar widget for all HallCrawl characters
 */
UCLASS()
class HALLCRAWL_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void UpdateValues(const class UAttributeSetBase* HealthAttributeSet) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Bar", meta=(BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Bar", meta=(BindWidget))
	UProgressBar* ShieldBar;
};
