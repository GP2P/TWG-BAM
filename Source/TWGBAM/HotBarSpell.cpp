// Fill out your copyright notice in the Description page of Project Settings.


#include "HotBarSpell.h"
#include <Components/ProgressBar.h>

void UHotBarSpell::SetUsage(float percent) {
	Usage->SetPercent(percent);
}

void UHotBarSpell::SetActiveBorder() {
	Active->SetOpacity(1.0f);
	Normal->SetOpacity(0.0f);
}

void UHotBarSpell::SetNormalBorder() {
	Active->SetOpacity(0.0f);
	Normal->SetOpacity(1.0f);
}
