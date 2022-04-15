// Fill out your copyright notice in the Description page of Project Settings.


#include "HotBar.h"
#include "Components/UniformGridPanel.h"

void UHotBar::AddSpell(UHotBarSpell* Spell) {
	if (col < 5) {
		UE_LOG(LogTemp, Warning, TEXT("Col: %d"), col);
		HotBar->AddChildToUniformGrid(Spell, 0, col);
		col++;
		//HotBar->AddChildToGrid(Spell, 0, 2);
	}
}