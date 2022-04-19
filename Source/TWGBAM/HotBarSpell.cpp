// Fill out your copyright notice in the Description page of Project Settings.


#include "HotBarSpell.h"
#include <Components/ProgressBar.h>

void UHotBarSpell::SetUsage(float percent) {
	Usage->SetPercent(percent);
}

