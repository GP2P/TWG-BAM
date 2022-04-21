// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HotBarSpell.h"
#include "Math/Color.h"
#include "HotBar.generated.h"

UCLASS()
class TWGBAM_API UHotBar : public UUserWidget
{
	GENERATED_BODY()

public:
	void AddFire(float NumFire);
	void AddThunder(float NumThunder);
	void AddWater(float NumWater);

	void UseFire(float Fpercent) { Fire->SetUsage(Fpercent); }
	void UseThunder(float Tpercent) { Thunder->SetUsage(Tpercent); }
	void UseWater(float Wpercent) { Water->SetUsage(Wpercent); }

	void OnFire();
	void OnThunder();
	void OnWater();

protected:
	UPROPERTY(meta = (BindWidget))
		class UGridPanel* HotBar;
	UPROPERTY(meta = (BindWidget))
		class UHotBarSpell* Thunder;
	UPROPERTY(meta = (BindWidget))
		class UHotBarSpell* Fire;
	UPROPERTY(meta = (BindWidget))
		class UHotBarSpell* Water;
	UPROPERTY(meta = (BindWidget))
		class UBorder* ThunderBorder;
	UPROPERTY(meta = (BindWidget))
		class UBorder* FireBorder;
	UPROPERTY(meta = (BindWidget))
		class UBorder* WaterBorder;

	FLinearColor TBorderColor = FLinearColor(1.0f, 0.923f, 0.0f, 1.0f);
	FLinearColor FBorderColor = FLinearColor(1.0f, 0.159f, 0.0f, 1.0f);
	FLinearColor WBorderColor = FLinearColor(0.0f, 0.152f, 1.0f, 1.0f);
	FLinearColor ABorderColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);

};
