// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/PokemonDamageExecution.h"
#include "AbilitySystem/Pokemon_AttributeSet.h"
#include "Pokemon/Pokemon_Types.h"
#include "Pokemon/Pokemon_Move.h"
#include "Pokemon/Pokemon_Pokemon.h"
#include "GameplayEffectTypes.h"

struct FDamageCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxHealth);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Attack);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defense);
	DECLARE_ATTRIBUTE_CAPTUREDEF(SpecialAttack);
	DECLARE_ATTRIBUTE_CAPTUREDEF(SpecialDefense);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Speed);

	FDamageCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPokemon_AttributeSet, Health, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPokemon_AttributeSet, MaxHealth, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPokemon_AttributeSet, Attack, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPokemon_AttributeSet, Defense, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPokemon_AttributeSet, SpecialAttack, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPokemon_AttributeSet, SpecialDefense, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPokemon_AttributeSet, Speed, Source, false);
	}
};

static const FDamageCapture& GetDamageCapture()
{
	static FDamageCapture Capture;
	return Capture;
}

UPokemonDamageExecution::UPokemonDamageExecution()
{
	const FDamageCapture& Capture = GetDamageCapture();
	RelevantAttributesToCapture.Add(Capture.HealthDef);
	RelevantAttributesToCapture.Add(Capture.MaxHealthDef);
	RelevantAttributesToCapture.Add(Capture.AttackDef);
	RelevantAttributesToCapture.Add(Capture.DefenseDef);
	RelevantAttributesToCapture.Add(Capture.SpecialAttackDef);
	RelevantAttributesToCapture.Add(Capture.SpecialDefenseDef);
	RelevantAttributesToCapture.Add(Capture.SpeedDef);
}

void UPokemonDamageExecution::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	if (!SourceASC || !TargetASC)
	{
		return;
	}

	const FDamageCapture& Capture = GetDamageCapture();

	// 读取双方属性
	float Attack = 0.0f;
	float Defense = 0.0f;
	float SpecialAttack = 0.0f;
	float SpecialDefense = 0.0f;

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(Capture.AttackDef, FAggregatorEvaluateParameters(), Attack);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(Capture.DefenseDef, FAggregatorEvaluateParameters(), Defense);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(Capture.SpecialAttackDef, FAggregatorEvaluateParameters(), SpecialAttack);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(Capture.SpecialDefenseDef, FAggregatorEvaluateParameters(), SpecialDefense);

	// 从 SetByCaller 读取技能参数
	const FGameplayEffectSpec* SpecPtr = ExecutionParams.GetOwningSpec();
	if (!SpecPtr) return;
	const FGameplayEffectSpec& Spec = *SpecPtr;
	static const FGameplayTag Tag_Power = FGameplayTag::RequestGameplayTag("Move.Power");
	static const FGameplayTag Tag_Type = FGameplayTag::RequestGameplayTag("Move.Type");
	static const FGameplayTag Tag_Category = FGameplayTag::RequestGameplayTag("Move.Category");

	float Power = Spec.GetSetByCallerMagnitude(Tag_Power, false, 0.0f);
	float TypeRaw = Spec.GetSetByCallerMagnitude(Tag_Type, false, 0.0f);
	float CatRaw = Spec.GetSetByCallerMagnitude(Tag_Category, false, 0.0f);

	EPokemonType MoveType = (EPokemonType)(int32)TypeRaw;
	EMoveCategory Category = (EMoveCategory)(int32)CatRaw;

	// 变化技能不造成伤害
	if (Category == EMoveCategory::Status || Power <= 0.0f)
	{
		return;
	}

	// 选择攻防属性
	float AtkValue = (Category == EMoveCategory::Special) ? SpecialAttack : Attack;
	float DefValue = (Category == EMoveCategory::Special) ? SpecialDefense : Defense;

	// 防止除以零
	if (DefValue <= 0.0f) DefValue = 1.0f;

	// 宝可梦伤害公式：((2 * Lv / 5 + 2) * Power * A / D / 50 + 2)
	const float Level = 50.0f;
	float Damage = ((2.0f * Level / 5.0f + 2.0f) * Power * AtkValue / DefValue / 50.0f + 2.0f);

	// 获取双方宝可梦
	AActor* SourceActor = SourceASC->GetAvatarActor();
	AActor* TargetActor = TargetASC->GetAvatarActor();

	// STAB
	if (APokemon_Pokemon* Attacker = Cast<APokemon_Pokemon>(SourceActor))
	{
		Damage *= GetSTABMultiplier(MoveType, Attacker->GetType1(), Attacker->GetType2());
	}

	// 属性克制
	if (APokemon_Pokemon* Defender = Cast<APokemon_Pokemon>(TargetActor))
	{
		Damage *= GetTypeEffectiveness(MoveType, Defender->GetType1(), Defender->GetType2());
	}

	// 随机修正 0.85~1.0
	Damage *= FMath::FRandRange(0.85f, 1.0f);

	// 确保最小伤害为 1
	if (Damage < 1.0f) Damage = 1.0f;

	// 输出：修改目标 Health
	FGameplayModifierEvaluatedData ModData;
	ModData.Attribute = Capture.HealthDef;
	ModData.ModifierOp = EGameplayModOp::Additive;
	ModData.Magnitude = -Damage;
	OutExecutionOutput.AddOutputModifier(ModData);
}
