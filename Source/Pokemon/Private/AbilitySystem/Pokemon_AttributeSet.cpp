// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Pokemon_AttributeSet.h"

#include "Net/UnrealNetwork.h"

UPokemon_AttributeSet::UPokemon_AttributeSet()
{
	InitHealth(0.0f);
	InitMaxHealth(0.0f);
	InitAttack(0.0f);
	InitDefense(0.0f);
	InitSpecialAttack(0.0f);
	InitSpecialDefense(0.0f);
	InitSpeed(0.0f);
}

void UPokemon_AttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UPokemon_AttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPokemon_AttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPokemon_AttributeSet, Attack, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPokemon_AttributeSet, Defense, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPokemon_AttributeSet, SpecialAttack, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPokemon_AttributeSet, SpecialDefense, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPokemon_AttributeSet, Speed, COND_None, REPNOTIFY_Always);
}

void UPokemon_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPokemon_AttributeSet, Health, OldHealth);
}

void UPokemon_AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPokemon_AttributeSet, MaxHealth, OldMaxHealth);
}

void UPokemon_AttributeSet::OnRep_Attack(const FGameplayAttributeData& OldAttack)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPokemon_AttributeSet, Attack, OldAttack);
}

void UPokemon_AttributeSet::OnRep_Defense(const FGameplayAttributeData& OldDefense)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPokemon_AttributeSet, Defense, OldDefense);
}

void UPokemon_AttributeSet::OnRep_SpecialAttack(const FGameplayAttributeData& OldSpecialAttack)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPokemon_AttributeSet, SpecialAttack, OldSpecialAttack);
}

void UPokemon_AttributeSet::OnRep_SpecialDefense(const FGameplayAttributeData& OldSpecialDefense)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPokemon_AttributeSet, SpecialDefense, OldSpecialDefense);
}

void UPokemon_AttributeSet::OnRep_Speed(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPokemon_AttributeSet, Speed, OldSpeed);
}

float UPokemon_AttributeSet::CalculateStat(int32 Base, int32 IV, int32 EV, int32 Level, bool bIsHP) const
{
	// 能力值 = (种族值 * 2 + 个体值 + 努力值/4) * 等级 / 100 + 5（非HP）或 + 等级 + 10（HP）
	float Stat = (Base * 2 + IV + EV / 4.0f) * Level / 100.0f;
	if (bIsHP)
		return Stat + Level + 10.0f;
	else
		return Stat + 5.0f;
}

void UPokemon_AttributeSet::CalculateStats(const FPokemonBaseStats& BaseStats, const FPokemonIVs& IVs, const FPokemonEVs& EVs)
{
	const int32 FixedLevel = 50;  // 固定等级50

	float NewMaxHealth = CalculateStat(BaseStats.HP, IVs.HP, EVs.HP, FixedLevel, true);
	float NewAttack = CalculateStat(BaseStats.Attack, IVs.Attack, EVs.Attack, FixedLevel, false);
	float NewDefense = CalculateStat(BaseStats.Defense, IVs.Defense, EVs.Defense, FixedLevel, false);
	float NewSpecialAttack = CalculateStat(BaseStats.SpecialAttack, IVs.SpecialAttack, EVs.SpecialAttack, FixedLevel, false);
	float NewSpecialDefense = CalculateStat(BaseStats.SpecialDefense, IVs.SpecialDefense, EVs.SpecialDefense, FixedLevel, false);
	float NewSpeed = CalculateStat(BaseStats.Speed, IVs.Speed, EVs.Speed, FixedLevel, false);

	SetMaxHealth(NewMaxHealth);
	SetHealth(NewMaxHealth);   // 初始满血
	SetAttack(NewAttack);
	SetDefense(NewDefense);
	SetSpecialAttack(NewSpecialAttack);
	SetSpecialDefense(NewSpecialDefense);
	SetSpeed(NewSpeed);
}
