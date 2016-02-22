// Fill out your copyright notice in the Description page of Project Settings.

#include "OHW2DBenStone.h"
#include "OHW2DBenStoneGameMode.h"
#include "MyCharacter.h"

AOHW2DBenStoneGameMode::AOHW2DBenStoneGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = AMyCharacter::StaticClass();
}


