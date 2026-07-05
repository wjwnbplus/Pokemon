// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/UI/BattleHUDWidget.h"
#include "Battle/BattleManager.h"
#include "Pokemon/Pokemon_Pokemon.h"
#include "Pokemon/Pokemon_Move.h"

void UBattleHUDWidget::OnFightSelected()
{
	if (BattleManager)
	{
		// 显示技能选择菜单（由蓝图实现 ShowMoveSelection）
		TArray<UPokemon_Move*> Moves;
		if (APokemon_Pokemon* PlayerPokemon = BattleManager->GetPlayerPokemon())
		{
			Moves = PlayerPokemon->GetMoveSet();
		}
		ShowMoveSelection(Moves);
	}
}

void UBattleHUDWidget::OnSwitchSelected()
{
	if (BattleManager)
	{
		// 显示宝可梦选择菜单（由蓝图实现 ShowPartySelection）
		// 简化：直接获取训练家的宝可梦队伍
		// 实际实现中需要从 BattleManager 获取训练家引用
		ShowPartySelection(TArray<APokemon_Pokemon*>());
	}
}

void UBattleHUDWidget::OnBagSelected()
{
	ShowMessage(FText::FromString(TEXT("Bag is not available yet!")));
}

void UBattleHUDWidget::OnRunSelected()
{
	if (BattleManager)
	{
		BattleManager->PlayerAttemptRun();
	}
}

void UBattleHUDWidget::OnMoveSelected(int32 MoveIndex)
{
	if (BattleManager)
	{
		BattleManager->PlayerSelectMove(MoveIndex);
	}
}

void UBattleHUDWidget::OnPokemonSelected(int32 PartyIndex)
{
	if (BattleManager)
	{
		BattleManager->PlayerSelectSwitch(PartyIndex);
	}
}
