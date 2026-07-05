// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Pokemon/Pokemon_Pokemon.h"
#include "Pokemon/Pokemon_Move.h"


APokemon_Pokemon::APokemon_Pokemon()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	AttributeSet = CreateDefaultSubobject<UPokemon_AttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* APokemon_Pokemon::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

float APokemon_Pokemon::GetHealth() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetHealth();
	}
	return 0.0f;
}

float APokemon_Pokemon::GetMaxHealth() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetMaxHealth();
	}
	return 0.0f;
}

bool APokemon_Pokemon::HasUsableMove() const
{
	for (const UPokemon_Move* Move : MoveSet)
	{
		if (Move && Move->CanUse())
		{
			return true;
		}
	}
	return false;
}

void APokemon_Pokemon::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitStats(UPokemon_AttributeSet::StaticClass(), nullptr);

		UPokemon_AttributeSet* AttrSet = const_cast<UPokemon_AttributeSet*>(AbilitySystemComponent->GetSet<UPokemon_AttributeSet>());
		if (AttrSet)
		{
			AttrSet->CalculateStats(BaseStats, IVs, EVs);
		}

		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	// 初始化所有技能的 PP
	for (UPokemon_Move* Move : MoveSet)
	{
		if (Move)
		{
			Move->RestorePP();
		}
	}
}
