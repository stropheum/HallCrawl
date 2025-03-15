// Copyright Epic Games, Inc. All Rights Reserved.

#include "HallCrawlGameMode.h"
#include "HallCrawlCharacter.h"
#include "UObject/ConstructorHelpers.h"

AHallCrawlGameMode::AHallCrawlGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
