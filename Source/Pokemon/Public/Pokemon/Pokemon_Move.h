// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Pokemon/Pokemon_Types.h"
#include "Pokemon_Move.generated.h"

UENUM(BlueprintType)
enum class EMoveCategory : uint8
{
	Physical UMETA(DisplayName = "Physical"),
	Special  UMETA(DisplayName = "Special"),
	Status   UMETA(DisplayName = "Status"),
};

UCLASS(BlueprintType)
class POKEMON_API UPokemon_Move : public UDataAsset
{
	GENERATED_BODY()

public:
	// === 技能标识 ===
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move|Info")
	FText MoveName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move|Info")
	FText Description;

	// === 战斗参数 ===
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move|Battle")
	EPokemonType Type = EPokemonType::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move|Battle")
	EMoveCategory Category = EMoveCategory::Physical;

	// 基础威力（0 = 变化技能）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move|Battle")
	int32 Power = 0;

	// 命中率 0.0~1.0（1.0 = 必中）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move|Battle")
	float Accuracy = 1.0f;

	// 最大 PP
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move|Battle")
	int32 MaxPP = 35;

	// 先制度（越高越先出手）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move|Battle")
	int32 Priority = 0;

	// === 运行时状态（不保存在数据资产中）===
	UPROPERTY(Transient)
	int32 CurrentPP = 0;

public:
	UFUNCTION(BlueprintCallable, Category = "Move")
	bool CanUse() const { return CurrentPP > 0; }

	UFUNCTION(BlueprintCallable, Category = "Move")
	void RestorePP() { CurrentPP = MaxPP; }

	UFUNCTION(BlueprintCallable, Category = "Move")
	void ConsumePP() { if (CurrentPP > 0) CurrentPP--; }
};
