// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Pokemon/Pokemon_Pokemon.h"


APokemon_Pokemon::APokemon_Pokemon()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);  // 如果需要网络复制

	// 创建属性集组件
	AttributeSet = CreateDefaultSubobject<UPokemon_AttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* APokemon_Pokemon::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void APokemon_Pokemon::BeginPlay()
{
	Super::BeginPlay();
	// 初始化 ASC 属性集（非常重要，让 ASC 知道 AttributeSet）
	if (AbilitySystemComponent)
	{
		// 方法1：使用 InitStats 自动创建属性集
		AbilitySystemComponent->InitStats(UPokemon_AttributeSet::StaticClass(), nullptr);

		// 获取刚创建的属性集实例
		UPokemon_AttributeSet* AttrSet = const_cast<UPokemon_AttributeSet*>(AbilitySystemComponent->GetSet<UPokemon_AttributeSet>());
		if (AttrSet)
		{
			AttrSet->CalculateStats(BaseStats, IVs, EVs);
		}

		// 必须调用，建立 ASC 与 Actor 的关联
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}
