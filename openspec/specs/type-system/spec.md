# type-system Specification

## Purpose
Define Pokemon types, type effectiveness chart, and STAB (Same-Type Attack Bonus). Used by `UPokemonDamageExecution` during damage calculation. Pure data/function, no GAS dependency.

## Architecture

```
EPokemonType enum
        │
        ▼
GetTypeEffectiveness(MoveType, DefenderType1, DefenderType2) → float (0.0 ~ 4.0)
        │
        ▼
UPokemonDamageExecution::Execute() — applies the multiplier
```

## Requirements

### Requirement: Pokemon types
Each Pokemon SHALL have one or two types stored as `EPokemonType Type1` and `EPokemonType Type2` on `APokemon_Pokemon`.

#### Scenario: Single-type
- `Type1 = Fire`, `Type2 = None`

#### Scenario: Dual-type
- `Type1 = Water`, `Type2 = Ground`

### Requirement: Type effectiveness
`GetTypeEffectiveness(MoveType, DefenderType1, DefenderType2)` returns a multiplier:
- 0.0 = No effect
- 0.25 = Double resisted
- 0.5 = Not very effective
- 1.0 = Normal
- 2.0 = Super effective
- 4.0 = Double super effective

#### Scenario: Super effective
- Fire vs Grass → 2.0x
- Ice vs Dragon/Ground → 2.0 × 2.0 = 4.0x

#### Scenario: Not very effective
- Fire vs Water → 0.5x
- Fire vs Water/Rock → 0.5 × 0.5 = 0.25x

#### Scenario: No effect
- Normal vs Ghost → 0.0x

### Requirement: STAB
If `MoveType` matches `AttackerType1` or `AttackerType2`, apply 1.5x multiplier.

#### Scenario: STAB applies
- Fire Pokemon uses Fire move → 1.5x

#### Scenario: No STAB
- Fire Pokemon uses Water move → no STAB

### Requirement: EPokemonType enum

```cpp
UENUM(BlueprintType)
enum class EPokemonType : uint8
{
    None     = 0,
    Normal   = 1,
    Fire     = 2,
    Water    = 3,
    Electric = 4,
    Grass    = 5,
    Ice      = 6,
    Fighting = 7,
    Poison   = 8,
    Ground   = 9,
    Flying   = 10,
    Psychic  = 11,
    Bug      = 12,
    Rock     = 13,
    Ghost    = 14,
    Dragon   = 15,
    Dark     = 16,
    Steel    = 17,
    Fairy    = 18,
};
```

## Technical Design

### File: Pokemon_Types.h

```cpp
#pragma once
#include "CoreMinimal.h"
#include "Pokemon_Types.generated.h"

UENUM(BlueprintType)
enum class EPokemonType : uint8 { ... };

// Returns effectiveness multiplier: 0.0, 0.25, 0.5, 1.0, 2.0, or 4.0
float GetTypeEffectiveness(EPokemonType MoveType, EPokemonType DefenderType1, EPokemonType DefenderType2);

// STAB check
float GetSTABMultiplier(EPokemonType MoveType, EPokemonType AttackerType1, EPokemonType AttackerType2);
```

### Implementation approach

The type chart is a static 2D array indexed by `EPokemonType`. Each entry is a float (0.0, 0.5, 1.0, or 2.0). The function multiplies effectiveness against both defender types.

```cpp
float GetTypeEffectiveness(EPokemonType Move, EPokemonType Def1, EPokemonType Def2)
{
    static const float Chart[19][19] = { /* full type chart */ };
    float Multiplier = 1.0f;
    if (Def1 != EPokemonType::None) Multiplier *= Chart[(int32)Move][(int32)Def1];
    if (Def2 != EPokemonType::None) Multiplier *= Chart[(int32)Move][(int32)Def2];
    return Multiplier;
}
```

### Integration with ExecutionCalculation

```cpp
void UPokemonDamageExecution::Execute_Implementation(...)
{
    // ... read attributes and SetByCaller values ...

    float Dmg = ((2.0f * 50 / 5.0f + 2.0f) * Power * Atk / Def / 50.0f + 2.0f);

    // STAB
    Dmg *= GetSTABMultiplier(MoveType, AttackerType1, AttackerType2);

    // Type effectiveness
    Dmg *= GetTypeEffectiveness(MoveType, DefenderType1, DefenderType2);

    // Random
    Dmg *= FMath::FRandRange(0.85f, 1.0f);

    OutOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
        TargetHealthAttr, EGameplayModOp::Add, -Dmg));
}
```

### Integration with APokemon_Pokemon

```cpp
// Added to APokemon_Pokemon:
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon|Type")
EPokemonType Type1 = EPokemonType::None;

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon|Type")
EPokemonType Type2 = EPokemonType::None;
```

## Type chart reference

Full chart in `Pokemon_Types.cpp`. Key highlights:

| Attack→Defense | 2x (Super Effective) | 0.5x (Not Effective) | 0x (No Effect) |
|---|---|---|---|
| Fire | Grass, Ice, Bug, Steel | Fire, Water, Rock, Dragon | — |
| Water | Fire, Ground, Rock | Water, Grass, Dragon | — |
| Electric | Water, Flying | Electric, Grass, Dragon | Ground |
| Grass | Water, Ground, Rock | Fire, Grass, Poison, Flying, Bug, Dragon, Steel | — |
| Normal | — | Rock, Steel | Ghost |
| Ground | Fire, Electric, Poison, Rock, Steel | Grass, Bug | Flying |
| Dragon | Dragon | Steel | Fairy |
| Ghost | Psychic, Ghost | Dark | Normal |
| Fairy | Fighting, Dragon, Dark | Fire, Poison, Steel | — |
