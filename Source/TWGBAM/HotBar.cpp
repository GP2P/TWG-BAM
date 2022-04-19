// Fill out your copyright notice in the Description page of Project Settings.


#include "HotBar.h"

void UHotBar::AddFire(float NumFire) {
	Fire->SetAmount(FText::AsNumber(NumFire));
}

void UHotBar::AddThunder(float NumThunder) {
	Thunder->SetAmount(FText::AsNumber(NumThunder));
}

void UHotBar::AddWater(float NumWater) {
	Water->SetAmount(FText::AsNumber(NumWater));
}