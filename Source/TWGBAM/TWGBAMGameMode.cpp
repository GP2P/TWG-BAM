// Copyright Epic Games, Inc. All Rights Reserved.

#include "TWGBAMGameMode.h"
#include "TWGBAMCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATWGBAMGameMode::ATWGBAMGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
