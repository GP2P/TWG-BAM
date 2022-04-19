// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "HotBarSpell.generated.h"

UCLASS()
class TWGBAM_API UHotBarSpell : public UUserWidget
{
	GENERATED_BODY()

public:
	FText GetAmount() { return Amount->GetText(); }
	void SetAmount(FText InText) { Amount->SetText(InText); }
	void SetUsage(float percent);

protected:
	UPROPERTY(meta = (BindWidget))
		class UImage* SpellImage;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Amount;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* Usage;
};