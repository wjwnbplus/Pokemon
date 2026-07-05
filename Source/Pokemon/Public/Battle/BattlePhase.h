// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattlePhase.generated.h"

UENUM(BlueprintType)
enum class EBattlePhase : uint8
{
	None         UMETA(DisplayName = "None"),
	Init         UMETA(DisplayName = "Init"),
	PlayerAction UMETA(DisplayName = "Player Action"),
	NPCAction    UMETA(DisplayName = "NPC Action"),
	Resolve      UMETA(DisplayName = "Resolve"),
	Victory      UMETA(DisplayName = "Victory"),
	Defeat       UMETA(DisplayName = "Defeat"),
};
