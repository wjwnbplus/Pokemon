// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Battle/BattlePhase.h"
#include "BattleManager.generated.h"

class APokemon_BaseCharacter;
class APokemon_Pokemon;
class UPokemon_Move;
class UBattleHUDWidget;

UCLASS()
class POKEMON_API ABattleManager : public AActor
{
	GENERATED_BODY()

public:
	ABattleManager();

	// 开始对战
	void StartBattle(APokemon_BaseCharacter* InPlayerTrainer, APokemon_BaseCharacter* InNPCTrainer);

	// === 玩家操作 ===
	UFUNCTION(BlueprintCallable, Category = "Battle")
	void PlayerSelectMove(int32 MoveIndex);

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void PlayerSelectSwitch(int32 PartyIndex);

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void PlayerAttemptRun();

	// === NPC 操作 ===
	void ExecuteNPCAction();

	// === 执行一个技能 ===
	void ExecuteMove(APokemon_Pokemon* Attacker, APokemon_Pokemon* Defender, UPokemon_Move* Move);

	// === 处理昏厥 ===
	void HandleFaint(APokemon_Pokemon* FaintedPokemon, bool bIsPlayer);

	// === 结束对战 ===
	void EndBattle(bool bPlayerWon);

	// === 获取当前状态 ===
	EBattlePhase GetBattlePhase() const { return BattlePhase; }
	APokemon_Pokemon* GetPlayerPokemon() const { return PlayerPokemon; }
	APokemon_Pokemon* GetNPCPokemon() const { return NPCPokemon; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	// 发送宝可梦出场
	void SendOutPokemon(bool bIsPlayer, APokemon_Pokemon* Pokemon);

	// 检查一方是否全部昏厥
	bool IsTeamDefeated(APokemon_BaseCharacter* Trainer) const;

	// 获取下一个可用宝可梦
	APokemon_Pokemon* GetNextAvailablePokemon(APokemon_BaseCharacter* Trainer) const;

	// 显示消息
	void ShowBattleMessage(const FString& Message);

	// === 对战双方 ===
	UPROPERTY()
	APokemon_BaseCharacter* PlayerTrainer;

	UPROPERTY()
	APokemon_BaseCharacter* NPCTrainer;

	// === 当前出战宝可梦 ===
	UPROPERTY()
	APokemon_Pokemon* PlayerPokemon;

	UPROPERTY()
	APokemon_Pokemon* NPCPokemon;

	// === 当前阶段 ===
	EBattlePhase BattlePhase;

	// === 玩家选择的技能索引 ===
	int32 SelectedMoveIndex;

	// === 对战 UI ===
	UPROPERTY()
	UBattleHUDWidget* HUDWidget;

	// === HUD Widget 类 ===
	UPROPERTY(EditAnywhere, Category = "Battle|UI")
	TSubclassOf<UBattleHUDWidget> HUDWidgetClass;
};
