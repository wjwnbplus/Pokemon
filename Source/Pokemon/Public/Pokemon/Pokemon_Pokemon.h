// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Pokemon_AttributeSet.h"
#include "GameFramework/Character.h"
#include "Pokemon/Pokemon_Types.h"
#include "Pokemon_Pokemon.generated.h"

class UPokemon_Move;

UCLASS()
class POKEMON_API APokemon_Pokemon : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APokemon_Pokemon();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// 获取属性集
	UFUNCTION(BlueprintCallable, Category = "Pokemon|Stats")
	const UPokemon_AttributeSet* GetPokemonAttributeSet() const { return AttributeSet; }

	// 获取当前 HP
	UFUNCTION(BlueprintCallable, Category = "Pokemon|Stats")
	float GetHealth() const;

	// 获取最大 HP
	UFUNCTION(BlueprintCallable, Category = "Pokemon|Stats")
	float GetMaxHealth() const;

	// 检查是否昏厥
	UFUNCTION(BlueprintCallable, Category = "Pokemon|Battle")
	bool IsFainted() const { return bIsFainted; }

	// 是否有可用技能
	UFUNCTION(BlueprintCallable, Category = "Pokemon|Battle")
	bool HasUsableMove() const;

	// === 公开访问器 ===
	FName GetPokemonName() const { return PokemonName; }
	EPokemonType GetType1() const { return Type1; }
	EPokemonType GetType2() const { return Type2; }
	const TArray<UPokemon_Move*>& GetMoveSet() const { return MoveSet; }
	void SetFainted(bool bFainted) { bIsFainted = bFainted; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UPokemon_AttributeSet* AttributeSet;

	// ===== 新增属性 =====

	// 宝可梦名称
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon|Identity", meta = (AllowPrivateAccess = "true"))
	FName PokemonName;

	// 属性1
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon|Type", meta = (AllowPrivateAccess = "true"))
	EPokemonType Type1 = EPokemonType::None;

	// 属性2
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon|Type", meta = (AllowPrivateAccess = "true"))
	EPokemonType Type2 = EPokemonType::None;

	// 技能列表
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon|Moves", meta = (AllowPrivateAccess = "true"))
	TArray<UPokemon_Move*> MoveSet;

	// 是否昏厥
	UPROPERTY(VisibleAnywhere, Category = "Pokemon|Battle")
	bool bIsFainted = false;

	// 种族值、个体值、努力值（用于初始化）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon|Stats", meta = (AllowPrivateAccess = "true"))
	FPokemonBaseStats BaseStats;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon|Stats", meta = (AllowPrivateAccess = "true"))
	FPokemonIVs IVs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon|Stats", meta = (AllowPrivateAccess = "true"))
	FPokemonEVs EVs;
};
