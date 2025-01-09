// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealEngineGameMode.h"
#include "UnrealEngineCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUnrealEngineGameMode::AUnrealEngineGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
