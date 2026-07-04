// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "Pokemon_AttributeSet.generated.h"


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class POKEMON_API UPokemon_AttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPokemon_AttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// 当前生命值（战斗中动态变化）
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UPokemon_AttributeSet, Health)

	// 最大生命值
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UPokemon_AttributeSet, MaxHealth)

	// 物理攻击
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Attack)
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(UPokemon_AttributeSet, Attack)

	// 物理防御
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Defense)
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(UPokemon_AttributeSet, Defense)

	// 特殊攻击
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_SpecialAttack)
	FGameplayAttributeData SpecialAttack;
	ATTRIBUTE_ACCESSORS(UPokemon_AttributeSet, SpecialAttack)

	// 特殊防御
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_SpecialDefense)
	FGameplayAttributeData SpecialDefense;
	ATTRIBUTE_ACCESSORS(UPokemon_AttributeSet, SpecialDefense)

	// 速度
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Speed)
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(UPokemon_AttributeSet, Speed)

	void CalculateStats(const struct FPokemonBaseStats& BaseStats, const struct FPokemonIVs& IVs, const struct FPokemonEVs& EVs);

protected:
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	virtual void OnRep_Attack(const FGameplayAttributeData& OldAttack);

	UFUNCTION()
	virtual void OnRep_Defense(const FGameplayAttributeData& OldDefense);

	UFUNCTION()
	virtual void OnRep_SpecialAttack(const FGameplayAttributeData& OldSpecialAttack);

	UFUNCTION()
	virtual void OnRep_SpecialDefense(const FGameplayAttributeData& OldSpecialDefense);

	UFUNCTION()
	virtual void OnRep_Speed(const FGameplayAttributeData& OldSpeed);

private:
	// 辅助函数：计算单项能力值（HP使用不同公式）
	float CalculateStat(int32 Base, int32 IV, int32 EV, int32 Level, bool bIsHP) const;

};

USTRUCT(BlueprintType)
struct FPokemonBaseStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Attack = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Defense = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpecialAttack = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpecialDefense = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Speed = 0;
};

USTRUCT(BlueprintType)
struct FPokemonIVs
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Attack = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Defense = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpecialAttack = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpecialDefense = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Speed = 0;
};

USTRUCT(BlueprintType)
struct FPokemonEVs
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Attack = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Defense = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpecialAttack = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpecialDefense = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Speed = 0;
};
