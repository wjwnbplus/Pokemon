// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/BattleManager.h"
#include "Battle/PokemonDamageExecution.h"
#include "Character/Pokemon_BaseCharacter.h"
#include "Pokemon/Pokemon_Pokemon.h"
#include "Pokemon/Pokemon_Move.h"
#include "AbilitySystem/Pokemon_AttributeSet.h"
#include "Battle/UI/BattleHUDWidget.h"
#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameplayEffect.h"

ABattleManager::ABattleManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
	BattlePhase = EBattlePhase::None;
	SelectedMoveIndex = -1;
}

void ABattleManager::BeginPlay()
{
	Super::BeginPlay();
}

void ABattleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABattleManager::StartBattle(APokemon_BaseCharacter* InPlayerTrainer, APokemon_BaseCharacter* InNPCTrainer)
{
	if (!InPlayerTrainer || !InNPCTrainer)
	{
		return;
	}

	PlayerTrainer = InPlayerTrainer;
	NPCTrainer = InNPCTrainer;

	// 派出第一只宝可梦
	PlayerPokemon = GetNextAvailablePokemon(PlayerTrainer);
	NPCPokemon = GetNextAvailablePokemon(NPCTrainer);

	if (!PlayerPokemon || !NPCPokemon)
	{
		return;
	}

	// 创建 UI
	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UBattleHUDWidget>(GetWorld(), HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->SetBattleManager(this);
			HUDWidget->AddToViewport();
		}
	}

	// 更新 UI
	SendOutPokemon(true, PlayerPokemon);
	SendOutPokemon(false, NPCPokemon);

	BattlePhase = EBattlePhase::PlayerAction;
	ShowBattleMessage(FString::Printf(TEXT("Go! %s!"), *PlayerPokemon->GetPokemonName().ToString()));
}

void ABattleManager::SendOutPokemon(bool bIsPlayer, APokemon_Pokemon* Pokemon)
{
	if (!Pokemon || !HUDWidget) return;

	const FString Name = Pokemon->GetPokemonName().ToString();
	float HP = Pokemon->GetHealth();
	float MaxHP = Pokemon->GetMaxHealth();

	if (bIsPlayer)
	{
		HUDWidget->UpdatePlayerHP(HP, MaxHP, Name);
	}
	else
	{
		HUDWidget->UpdateNPCHP(HP, MaxHP, Name);
	}
}

void ABattleManager::PlayerSelectMove(int32 MoveIndex)
{
	if (BattlePhase != EBattlePhase::PlayerAction) return;
	if (!PlayerPokemon || !NPCPokemon) return;

	SelectedMoveIndex = MoveIndex;

	// 先执行玩家技能
	UPokemon_Move* SelectedMove = nullptr;
	const TArray<UPokemon_Move*>& Moves = PlayerPokemon->GetMoveSet();
	if (Moves.IsValidIndex(MoveIndex))
	{
		SelectedMove = Moves[MoveIndex];
	}

	if (SelectedMove)
	{
		BattlePhase = EBattlePhase::Resolve;
		ExecuteMove(PlayerPokemon, NPCPokemon, SelectedMove);

		// 如果对战未结束，执行 NPC 行动
		if (BattlePhase != EBattlePhase::Victory && BattlePhase != EBattlePhase::Defeat)
		{
			BattlePhase = EBattlePhase::NPCAction;
			ExecuteNPCAction();
		}
	}
	else
	{
		BattlePhase = EBattlePhase::PlayerAction;
	}
}

void ABattleManager::PlayerSelectSwitch(int32 PartyIndex)
{
	if (BattlePhase != EBattlePhase::PlayerAction) return;
	if (!PlayerTrainer) return;

	TArray<APokemon_Pokemon*> Party = PlayerTrainer->GetPokemonParty();
	if (!Party.IsValidIndex(PartyIndex)) return;

	APokemon_Pokemon* NewPokemon = Party[PartyIndex];
	if (!NewPokemon || NewPokemon->IsFainted()) return;

	// 换宝可梦
	PlayerPokemon = NewPokemon;
	PlayerTrainer->SetCurrentBattlePokemon(PlayerPokemon);
	SendOutPokemon(true, PlayerPokemon);

	ShowBattleMessage(FString::Printf(TEXT("Come back! Go! %s!"), *PlayerPokemon->GetPokemonName().ToString()));

	// 对手攻击（换人惩罚）
	BattlePhase = EBattlePhase::NPCAction;
	ExecuteNPCAction();
}

void ABattleManager::PlayerAttemptRun()
{
	if (BattlePhase != EBattlePhase::PlayerAction) return;

	// 逃跑判定：比较双方速度
	float PlayerSpeed = PlayerPokemon ? PlayerPokemon->GetPokemonAttributeSet()->GetSpeed() : 0.0f;
	float NPCSpeed = NPCPokemon ? NPCPokemon->GetPokemonAttributeSet()->GetSpeed() : 0.0f;

	// 速度大于对手时必定逃跑，否则 50% 概率
	bool bCanFlee = (PlayerSpeed > NPCSpeed) || (FMath::FRand() < 0.5f);

	if (bCanFlee)
	{
		ShowBattleMessage(TEXT("You fled successfully!"));
		EndBattle(false); // 逃跑不算胜利
	}
	else
	{
		ShowBattleMessage(TEXT("Can't escape!"));
		BattlePhase = EBattlePhase::NPCAction;
		ExecuteNPCAction();
	}
}

void ABattleManager::ExecuteNPCAction()
{
	if (!NPCPokemon) return;

	// NPC 随机选择可用技能
	TArray<UPokemon_Move*> AvailableMoves;
	for (UPokemon_Move* Move : NPCPokemon->GetMoveSet())
	{
		if (Move && Move->CanUse())
		{
			AvailableMoves.Add(Move);
		}
	}

	if (AvailableMoves.Num() == 0)
	{
		// 没有可用技能时使用 Struggle
		ShowBattleMessage(FString::Printf(TEXT("%s used Struggle!"), *NPCPokemon->GetPokemonName().ToString()));

		// Struggle：造成 1/4 最大 HP 的反伤给自己
		float StruggleDamage = NPCPokemon->GetMaxHealth() * 0.25f;
		// 通过 GE 扣自己的血
		UGameplayEffect* StruggleGE = NewObject<UGameplayEffect>(GetTransientPackage());
		StruggleGE->DurationPolicy = EGameplayEffectDurationType::Instant;
		FGameplayModifierInfo ModInfo;
		ModInfo.Attribute = UPokemon_AttributeSet::GetHealthAttribute();
		ModInfo.ModifierOp = EGameplayModOp::Additive;
		ModInfo.ModifierMagnitude = FScalableFloat(-StruggleDamage);
		StruggleGE->Modifiers.Add(ModInfo);

		UAbilitySystemComponent* NPCASC = NPCPokemon->GetAbilitySystemComponent();
		if (NPCASC)
		{
			FGameplayEffectSpecHandle StruggleSpec = NPCASC->MakeOutgoingSpec(StruggleGE, 1, NPCASC->MakeEffectContext());
			if (StruggleSpec.IsValid())
			{
				NPCASC->ApplyGameplayEffectSpecToTarget(*StruggleSpec.Data.Get(), NPCASC);
			}
		}

		// 更新 UI 并检查昏厥
		if (HUDWidget)
		{
			HUDWidget->UpdateNPCHP(NPCPokemon->GetHealth(), NPCPokemon->GetMaxHealth(), NPCPokemon->GetPokemonName().ToString());
		}

		if (NPCPokemon->GetHealth() <= 0.0f)
		{
			HandleFaint(NPCPokemon, false);
			return;
		}

		BattlePhase = EBattlePhase::PlayerAction;
		if (HUDWidget) HUDWidget->ShowMainMenu();
		return;
	}

	int32 RandomIndex = FMath::RandRange(0, AvailableMoves.Num() - 1);
	UPokemon_Move* SelectedMove = AvailableMoves[RandomIndex];

	// 执行 NPC 技能（NPC 攻击玩家）
	BattlePhase = EBattlePhase::Resolve;
	ExecuteMove(NPCPokemon, PlayerPokemon, SelectedMove);
}

void ABattleManager::ExecuteMove(APokemon_Pokemon* Attacker, APokemon_Pokemon* Defender, UPokemon_Move* Move)
{
	if (!Attacker || !Defender || !Move)
	{
		BattlePhase = EBattlePhase::PlayerAction;
		return;
	}

	// 1. 检查 PP
	if (!Move->CanUse())
	{
		ShowBattleMessage(TEXT("But no PP left!"));
		BattlePhase = EBattlePhase::PlayerAction;
		return;
	}

	// 2. 命中判定
	if (FMath::FRand() > Move->Accuracy)
	{
		ShowBattleMessage(FString::Printf(TEXT("%s used %s!"), *Attacker->GetPokemonName().ToString(), *Move->MoveName.ToString()));
		ShowBattleMessage(TEXT("But it missed!"));
		BattlePhase = EBattlePhase::PlayerAction;
		return;
	}

	// 3. 消耗 PP
	Move->ConsumePP();

	// 4. 显示技能使用消息
	ShowBattleMessage(FString::Printf(TEXT("%s used %s!"), *Attacker->GetPokemonName().ToString(), *Move->MoveName.ToString()));

	// 5. 变化技能
	if (Move->Category == EMoveCategory::Status)
	{
		ShowBattleMessage(TEXT("Status moves not yet implemented!"));
		BattlePhase = EBattlePhase::PlayerAction;
		return;
	}

	// 6. 创建 GE 并应用
	UAbilitySystemComponent* SourceASC = Attacker->GetAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = Defender->GetAbilitySystemComponent();

	if (!SourceASC || !TargetASC)
	{
		BattlePhase = EBattlePhase::PlayerAction;
		return;
	}

	UGameplayEffect* DamageGE = NewObject<UGameplayEffect>(GetTransientPackage());
	DamageGE->DurationPolicy = EGameplayEffectDurationType::Instant;

	// 绑定 ExecutionCalculation
	FGameplayEffectExecutionDefinition& ExecDef = DamageGE->Executions.AddDefaulted_GetRef();
	ExecDef.CalculationClass = UPokemonDamageExecution::StaticClass();

	// 创建 Spec（使用 MakeOutgoingSpec，传入 GE 的 Class）
	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
		DamageGE, 1, SourceASC->MakeEffectContext());
	if (!SpecHandle.IsValid())
	{
		BattlePhase = EBattlePhase::PlayerAction;
		return;
	}

	// 通过 SetByCaller 传递技能参数
	static const FGameplayTag Tag_Power = FGameplayTag::RequestGameplayTag("Move.Power");
	static const FGameplayTag Tag_Type = FGameplayTag::RequestGameplayTag("Move.Type");
	static const FGameplayTag Tag_Category = FGameplayTag::RequestGameplayTag("Move.Category");

	SpecHandle.Data->SetSetByCallerMagnitude(Tag_Power, (float)Move->Power);
	SpecHandle.Data->SetSetByCallerMagnitude(Tag_Type, (float)(int32)Move->Type);
	SpecHandle.Data->SetSetByCallerMagnitude(Tag_Category, (float)(int32)Move->Category);

	// 应用 GE 到目标
	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

	// 7. 更新 UI
	float DefenderHP = Defender->GetHealth();
	float DefenderMaxHP = Defender->GetMaxHealth();

	// 判断是谁在攻击
	bool bIsPlayerAttacking = (Attacker == PlayerPokemon);
	if (bIsPlayerAttacking)
	{
		HUDWidget->UpdateNPCHP(DefenderHP, DefenderMaxHP, Defender->GetPokemonName().ToString());
	}
	else
	{
		HUDWidget->UpdatePlayerHP(DefenderHP, DefenderMaxHP, Defender->GetPokemonName().ToString());
	}

	// 8. 检查昏厥
	if (DefenderHP <= 0.0f)
	{
		HandleFaint(Defender, (Defender == PlayerPokemon));
	}
	else
	{
		// 继续下一回合
		BattlePhase = EBattlePhase::PlayerAction;
		if (HUDWidget)
		{
			HUDWidget->ShowMainMenu();
		}
	}
}

void ABattleManager::HandleFaint(APokemon_Pokemon* FaintedPokemon, bool bIsPlayer)
{
	if (!FaintedPokemon) return;

	FaintedPokemon->SetFainted(true);
	ShowBattleMessage(FString::Printf(TEXT("%s fainted!"), *FaintedPokemon->GetPokemonName().ToString()));

	if (bIsPlayer)
	{
		// 玩家宝可梦昏厥
		APokemon_Pokemon* NextPokemon = GetNextAvailablePokemon(PlayerTrainer);
		if (NextPokemon)
		{
			PlayerPokemon = NextPokemon;
			PlayerTrainer->SetCurrentBattlePokemon(PlayerPokemon);
			SendOutPokemon(true, PlayerPokemon);
			ShowBattleMessage(FString::Printf(TEXT("Go! %s!"), *PlayerPokemon->GetPokemonName().ToString()));
			BattlePhase = EBattlePhase::PlayerAction;
			if (HUDWidget) HUDWidget->ShowMainMenu();
		}
		else
		{
			// 玩家全部昏厥
			EndBattle(false);
		}
	}
	else
	{
		// NPC 宝可梦昏厥
		APokemon_Pokemon* NextPokemon = GetNextAvailablePokemon(NPCTrainer);
		if (NextPokemon)
		{
			NPCPokemon = NextPokemon;
			NPCTrainer->SetCurrentBattlePokemon(NPCPokemon);
			SendOutPokemon(false, NPCPokemon);
			ShowBattleMessage(FString::Printf(TEXT("NPC sent out %s!"), *NPCPokemon->GetPokemonName().ToString()));
			BattlePhase = EBattlePhase::PlayerAction;
			if (HUDWidget) HUDWidget->ShowMainMenu();
		}
		else
		{
			// NPC 全部昏厥，玩家胜利
			EndBattle(true);
		}
	}
}

void ABattleManager::EndBattle(bool bPlayerWon)
{
	BattlePhase = bPlayerWon ? EBattlePhase::Victory : EBattlePhase::Defeat;

	if (HUDWidget)
	{
		if (bPlayerWon)
		{
			HUDWidget->ShowVictory();
		}
		else
		{
			HUDWidget->ShowDefeat();
		}
	}

	// 恢复所有技能的 PP
	TArray<APokemon_Pokemon*> AllPokemon;
	AllPokemon.Append(PlayerTrainer->GetPokemonParty());
	AllPokemon.Append(NPCTrainer->GetPokemonParty());

	for (APokemon_Pokemon* Pokemon : AllPokemon)
	{
		if (Pokemon)
		{
			for (UPokemon_Move* Move : Pokemon->GetMoveSet())
			{
				if (Move)
				{
					Move->RestorePP();
				}
			}
			Pokemon->SetFainted(false);
		}
	}

	// 延迟销毁对战管理器
	SetLifeSpan(3.0f);
}

bool ABattleManager::IsTeamDefeated(APokemon_BaseCharacter* Trainer) const
{
	for (APokemon_Pokemon* Pokemon : Trainer->GetPokemonParty())
	{
		if (Pokemon && !Pokemon->IsFainted())
		{
			return false;
		}
	}
	return true;
}

APokemon_Pokemon* ABattleManager::GetNextAvailablePokemon(APokemon_BaseCharacter* Trainer) const
{
	for (APokemon_Pokemon* Pokemon : Trainer->GetPokemonParty())
	{
		if (Pokemon && !Pokemon->IsFainted())
		{
			return Pokemon;
		}
	}
	return nullptr;
}

void ABattleManager::ShowBattleMessage(const FString& Message)
{
	if (HUDWidget)
	{
		HUDWidget->ShowMessage(FText::FromString(Message));
	}
}
