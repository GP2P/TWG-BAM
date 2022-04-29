// Fill out your copyright notice in the Description page of Project Settings.


#include "HotBar.h"
#include "Components/Border.h"

void UHotBar::AddFire(float NumFire) {
	Fire->SetAmount(FText::AsNumber(NumFire));
}

void UHotBar::AddThunder(float NumThunder) {
	Thunder->SetAmount(FText::AsNumber(NumThunder));
}

void UHotBar::AddWater(float NumWater) {
	Water->SetAmount(FText::AsNumber(NumWater));
}

void UHotBar::OnFire() {
	FireBorder->SetBrushColor(FBorderColor);
	ThunderBorder->SetBrushColor(ABorderColor);
	WaterBorder->SetBrushColor(ABorderColor);
}

void UHotBar::OnThunder() {
	ThunderBorder->SetBrushColor(TBorderColor);
	FireBorder->SetBrushColor(ABorderColor);
	WaterBorder->SetBrushColor(ABorderColor);
}

void UHotBar::OnWater() {
	WaterBorder->SetBrushColor(WBorderColor);
	FireBorder->SetBrushColor(ABorderColor);
	ThunderBorder->SetBrushColor(ABorderColor);
}

void UHotBar::SpellsCooling(bool fc, bool tc, bool wc, float cooldown) {
	FireCooling = fc;
	ThunderCooling = tc;
	WaterCooling = wc;
	cdTime = cooldown;
}