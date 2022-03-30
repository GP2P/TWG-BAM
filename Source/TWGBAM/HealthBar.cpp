
#include "HealthBar.h"
#include <Components/ProgressBar.h>

void UHealthBar::NativeTick(const FGeometry& myGeometry, float InDeltaTime) {
	Super::NativeTick(myGeometry, InDeltaTime);

	if (!Owner.IsValid())
		return;

	HealthBar->SetPercent(Owner->getHealth() / Owner->getMaxHealth());
}