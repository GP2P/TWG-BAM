// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TWGBAMCharacter.h"
#include "HealthBar.generated.h"


UCLASS(Abstract)
class TWGBAM_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	void setOwner(ATWGBAMCharacter* playerChar) { Owner = playerChar; }

protected:
	void NativeTick(const FGeometry& myGeometry, float InDeltaTime) override;

	TWeakObjectPtr<ATWGBAMCharacter> Owner;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* HealthBar;

};
