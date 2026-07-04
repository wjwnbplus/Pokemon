// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Pokemon_BaseCharacter.generated.h"

class APokemon_Pokemon;

UCLASS()
class POKEMON_API APokemon_BaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APokemon_BaseCharacter();

	// ========== 通用逻辑：宝可梦管理 ==========
	// 添加宝可梦到队伍
	UFUNCTION(BlueprintCallable, Category = "Trainer|Pokemon")
	void AddPokemon(APokemon_Pokemon* NewPokemon);

	// 设置当前出战宝可梦
	UFUNCTION(BlueprintCallable, Category = "Trainer|Pokemon")
	void SetCurrentBattlePokemon(APokemon_Pokemon* Pokemon);

	// 获取当前出战宝可梦
	UFUNCTION(BlueprintCallable, Category = "Trainer|Pokemon")
	APokemon_Pokemon* GetCurrentBattlePokemon() const { return CurrentBattlePokemon; }

	// 获取宝可梦队伍
	UFUNCTION(BlueprintCallable, Category = "Trainer|Pokemon")
	TArray<APokemon_Pokemon*> GetPokemonParty() const { return PokemonParty; }

protected:
	virtual void BeginPlay() override;
	
private:

	// ========== 通用属性 ==========
	// 训练家的宝可梦队伍
	UPROPERTY(EditAnywhere, Category = "Trainer|Pokemon")
	TArray<APokemon_Pokemon*> PokemonParty;

	// 当前出战的宝可梦
	UPROPERTY(EditAnywhere, Category = "Trainer|Pokemon")
	APokemon_Pokemon* CurrentBattlePokemon;
};
