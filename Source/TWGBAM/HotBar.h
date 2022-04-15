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
	void AddSpell(UHotBarSpell* Spell);

protected:
	UPROPERTY(meta = (BindWidget))
		class UUniformGridPanel* HotBar;

	int32 col = 0;

};
