// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HotBarSpell.h"
#include "HotBar.generated.h"

UCLASS()
class TWGBAM_API UHotBar : public UUserWidget
{
	GENERATED_BODY()

public:
	void AddFire(float NumFire);
	void AddThunder(float NumThunder);
	void AddWater(float NumWater);

protected:
	UPROPERTY(meta = (BindWidget))
		class UGridPanel* HotBar;
	UPROPERTY(meta = (BindWidget))
		class UHotBarSpell* Thunder;
	UPROPERTY(meta = (BindWidget))
		class UHotBarSpell* Fire;
	UPROPERTY(meta = (BindWidget))
		class UHotBarSpell* Water;

	int32 col = 0;

};
