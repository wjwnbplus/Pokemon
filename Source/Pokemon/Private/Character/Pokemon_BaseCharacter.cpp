// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Character/Pokemon_BaseCharacter.h"


// Sets default values
APokemon_BaseCharacter::APokemon_BaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APokemon_BaseCharacter::AddPokemon(APokemon_Pokemon* NewPokemon)
{
	if (NewPokemon && !PokemonParty.Contains(NewPokemon))
	{
		PokemonParty.Add(NewPokemon);
	}
}

void APokemon_BaseCharacter::SetCurrentBattlePokemon(APokemon_Pokemon* Pokemon)
{
	if (Pokemon && PokemonParty.Contains(Pokemon))
	{
		CurrentBattlePokemon = Pokemon;
	}
}

// Called when the game starts or when spawned
void APokemon_BaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}
