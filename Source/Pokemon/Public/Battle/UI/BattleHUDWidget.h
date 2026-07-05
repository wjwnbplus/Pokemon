// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleHUDWidget.generated.h"

class ABattleManager;
class APokemon_Pokemon;
class UPokemon_Move;

UCLASS()
class POKEMON_API UBattleHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetBattleManager(ABattleManager* InManager) { BattleManager = InManager; }

	// === UI 更新（BlueprintImplementableEvent = 在蓝图中实现）===
	UFUNCTION(BlueprintImplementableEvent, Category = "BattleUI")
	void ShowMainMenu();

	UFUNCTION(BlueprintImplementableEvent, Category = "BattleUI")
	void ShowMoveSelection(const TArray<UPokemon_Move*>& Moves);

	UFUNCTION(BlueprintImplementableEvent, Category = "BattleUI")
	void ShowPartySelection(const TArray<APokemon_Pokemon*>& Party);

	UFUNCTION(BlueprintImplementableEvent, Category = "BattleUI")
	void UpdatePlayerHP(float CurrentHP, float MaxHP, const FString& Name);

	UFUNCTION(BlueprintImplementableEvent, Category = "BattleUI")
	void UpdateNPCHP(float CurrentHP, float MaxHP, const FString& Name);

	UFUNCTION(BlueprintImplementableEvent, Category = "BattleUI")
	void ShowMessage(const FText& Message);

	UFUNCTION(BlueprintImplementableEvent, Category = "BattleUI")
	void ShowVictory();

	UFUNCTION(BlueprintImplementableEvent, Category = "BattleUI")
	void ShowDefeat();

	// === 按钮回调（由蓝图按钮点击调用）===
	UFUNCTION(BlueprintCallable, Category = "BattleUI")
	void OnFightSelected();

	UFUNCTION(BlueprintCallable, Category = "BattleUI")
	void OnSwitchSelected();

	UFUNCTION(BlueprintCallable, Category = "BattleUI")
	void OnBagSelected();

	UFUNCTION(BlueprintCallable, Category = "BattleUI")
	void OnRunSelected();

	UFUNCTION(BlueprintCallable, Category = "BattleUI")
	void OnMoveSelected(int32 MoveIndex);

	UFUNCTION(BlueprintCallable, Category = "BattleUI")
	void OnPokemonSelected(int32 PartyIndex);

private:
	UPROPERTY()
	ABattleManager* BattleManager = nullptr;
};
