// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pokemon_Types.generated.h"

UENUM(BlueprintType)
enum class EPokemonType : uint8
{
	None     UMETA(DisplayName = "None"),
	Normal   UMETA(DisplayName = "Normal"),
	Fire     UMETA(DisplayName = "Fire"),
	Water    UMETA(DisplayName = "Water"),
	Electric UMETA(DisplayName = "Electric"),
	Grass    UMETA(DisplayName = "Grass"),
	Ice      UMETA(DisplayName = "Ice"),
	Fighting UMETA(DisplayName = "Fighting"),
	Poison   UMETA(DisplayName = "Poison"),
	Ground   UMETA(DisplayName = "Ground"),
	Flying   UMETA(DisplayName = "Flying"),
	Psychic  UMETA(DisplayName = "Psychic"),
	Bug      UMETA(DisplayName = "Bug"),
	Rock     UMETA(DisplayName = "Rock"),
	Ghost    UMETA(DisplayName = "Ghost"),
	Dragon   UMETA(DisplayName = "Dragon"),
	Dark     UMETA(DisplayName = "Dark"),
	Steel    UMETA(DisplayName = "Steel"),
	Fairy    UMETA(DisplayName = "Fairy"),
};

// Returns type effectiveness multiplier: 0.0, 0.25, 0.5, 1.0, 2.0, or 4.0
POKEMON_API float GetTypeEffectiveness(EPokemonType MoveType, EPokemonType DefenderType1, EPokemonType DefenderType2);

// Returns STAB multiplier: 1.5 if MoveType matches either attacker type, else 1.0
POKEMON_API float GetSTABMultiplier(EPokemonType MoveType, EPokemonType AttackerType1, EPokemonType AttackerType2);
