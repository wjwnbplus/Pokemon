// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "PokemonDamageExecution.generated.h"

UCLASS()
class POKEMON_API UPokemonDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UPokemonDamageExecution();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
