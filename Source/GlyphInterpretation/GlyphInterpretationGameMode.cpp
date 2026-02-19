// Copyright Epic Games, Inc. All Rights Reserved.

#include "GlyphInterpretationGameMode.h"
#include "GlyphInterpretationCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGlyphInterpretationGameMode::AGlyphInterpretationGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
