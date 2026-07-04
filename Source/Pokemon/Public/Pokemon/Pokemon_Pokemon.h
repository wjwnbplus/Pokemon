// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Pokemon_AttributeSet.h"
#include "GameFramework/Character.h"
#include "Pokemon_Pokemon.generated.h"

UCLASS()
class POKEMON_API APokemon_Pokemon : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APokemon_Pokemon();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemComponent;

	// 属性集（实际存储数值）
	UPROPERTY()
	UPokemon_AttributeSet* AttributeSet;

	// 种族值、个体值、努力值（用于初始化）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon|Stats", meta = (AllowPrivateAccess = "true"))
	FPokemonBaseStats BaseStats;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon|Stats", meta = (AllowPrivateAccess = "true"))
	FPokemonIVs IVs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon|Stats", meta = (AllowPrivateAccess = "true"))
	FPokemonEVs EVs;
	
};
