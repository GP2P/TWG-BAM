
#include "HealthBar.h"
#include <Components/ProgressBar.h>

void UHealthBar::NativeTick(const FGeometry& myGeometry, float InDeltaTime) {
	Super::NativeTick(myGeometry, InDeltaTime);

	if (!Owner.IsValid()){
		return;
		UE_LOG(LogTemp, Warning, TEXT("owner not valid"));
	}

	HealthBar->SetPercent(Owner->getHealth() / Owner->getMaxHealth());
}