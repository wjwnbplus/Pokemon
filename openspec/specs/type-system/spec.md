# type-system Specification

## Purpose
Define Pokemon types, type effectiveness chart, and Same-Type Attack Bonus (STAB) mechanics.

## Requirements

### Requirement: Pokemon types
Each Pokemon SHALL have one or two types defined by the `EPokemonType` enum.

#### Scenario: Single-type Pokemon
- GIVEN a Pokemon has only one type
- THEN Type1 is set and Type2 is None

#### Scenario: Dual-type Pokemon
- GIVEN a Pokemon has two types
- THEN both Type1 and Type2 are set to valid types

### Requirement: Type effectiveness
The system SHALL calculate type effectiveness multipliers based on the classic Pokemon type chart.

#### Scenario: Super effective
- GIVEN a move's type is strong against the defender's type
- THEN apply a 2x multiplier
- WHEN both of the defender's types are weak to the move
- THEN apply a 4x multiplier (2x × 2x)

#### Scenario: Not very effective
- GIVEN a move's type is weak against the defender's type
- THEN apply a 0.5x multiplier
- WHEN both of the defender's types resist the move
- THEN apply a 0.25x multiplier (0.5x × 0.5x)

#### Scenario: No effect
- GIVEN a move's type has no effect on the defender's type
- THEN apply a 0x multiplier
- AND the move deals no damage

### Requirement: STAB (Same-Type Attack Bonus)
The system SHALL apply a 1.5x damage multiplier when the move's type matches either of the attacker's types.

#### Scenario: STAB applied
- GIVEN a Fire-type Pokemon uses a Fire-type move
- THEN apply 1.5x STAB multiplier

#### Scenario: No STAB
- GIVEN a Fire-type Pokemon uses a Water-type move
- THEN do NOT apply STAB multiplier

### Requirement: Type enum
The system SHALL define the following `EPokemonType` enum:

| Type | Enum Value |
|---|---|
| None | 0 |
| Normal | 1 |
| Fire | 2 |
| Water | 3 |
| Electric | 4 |
| Grass | 5 |
| Ice | 6 |
| Fighting | 7 |
| Poison | 8 |
| Ground | 9 |
| Flying | 10 |
| Psychic | 11 |
| Bug | 12 |
| Rock | 13 |
| Ghost | 14 |
| Dragon | 15 |
| Dark | 16 |
| Steel | 17 |
| Fairy | 18 |

## Technical Design

### New enum

```cpp
UENUM(BlueprintType)
enum class EPokemonType : uint8
{
    None     UMETA(DisplayName = "None"),
    Normal   UMETA(DisplayName = "Normal"),
    Fire     UMETA(DisplayName = "Fire"),
    Water    UMETA(DisplayName = "Water"),
    Electric UMETA(DisplayName = "Electric"),
    Grass    UMETA(DisplayName = "Grass"),
    Ice      UMETA(DisplayName = "Ice"),
    Fighting UMETA(DisplayName = "Fighting"),
    Poison   UMETA(DisplayName = "Poison"),
    Ground   UMETA(DisplayName = "Ground"),
    Flying   UMETA(DisplayName = "Flying"),
    Psychic  UMETA(DisplayName = "Psychic"),
    Bug      UMETA(DisplayName = "Bug"),
    Rock     UMETA(DisplayName = "Rock"),
    Ghost    UMETA(DisplayName = "Ghost"),
    Dragon   UMETA(DisplayName = "Dragon"),
    Dark     UMETA(DisplayName = "Dark"),
    Steel    UMETA(DisplayName = "Steel"),
    Fairy    UMETA(DisplayName = "Fairy"),
};
```

### Type effectiveness table

```cpp
// Returns multiplier: 0.0, 0.25, 0.5, 1.0, 2.0, 4.0
float GetTypeEffectiveness(EPokemonType MoveType, EPokemonType DefenderType1, EPokemonType DefenderType2);
```

The function SHALL implement the classic type chart. Key entries:

| Move Type → Defender | Super Effective (2x) | Not Effective (0.5x) | No Effect (0x) |
|---|---|---|---|
| Normal | — | Rock, Steel | Ghost |
| Fire | Grass, Ice, Bug, Steel | Fire, Water, Rock, Dragon | — |
| Water | Fire, Ground, Rock | Water, Grass, Dragon | — |
| Electric | Water, Flying | Electric, Grass, Dragon | Ground |
| Grass | Water, Ground, Rock | Fire, Grass, Poison, Flying, Bug, Dragon, Steel | — |
| Ice | Grass, Ground, Flying, Dragon | Fire, Water, Ice, Steel | — |
| Fighting | Normal, Ice, Rock, Dark, Steel | Poison, Flying, Psychic, Bug, Fairy | Ghost |
| Poison | Grass, Fairy | Poison, Ground, Rock, Ghost | Steel |
| Ground | Fire, Electric, Poison, Rock, Steel | Grass, Bug | Flying |
| Flying | Grass, Fighting, Bug | Electric, Rock, Steel | — |
| Psychic | Fighting, Poison | Psychic, Steel | Dark |
| Bug | Grass, Psychic, Dark | Fire, Fighting, Poison, Flying, Ghost, Steel, Fairy | — |
| Rock | Fire, Ice, Flying, Bug | Fighting, Ground, Steel | — |
| Ghost | Psychic, Ghost | Dark | Normal |
| Dragon | Dragon | Steel | Fairy |
| Dark | Psychic, Ghost | Fighting, Dark, Fairy | — |
| Steel | Ice, Rock, Fairy | Fire, Water, Electric, Steel | — |
| Fairy | Fighting, Dragon, Dark | Fire, Poison, Steel | — |

### Integration with APokemon_Pokemon

Add to `APokemon_Pokemon`:
```cpp
UPROPERTY(EditAnywhere, Category = "Pokemon|Type")
EPokemonType Type1;

UPROPERTY(EditAnywhere, Category = "Pokemon|Type")
EPokemonType Type2;
```
