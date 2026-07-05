# pokemon-moves Specification

## Purpose
Define the Pokemon move (skill) system, including move data structure, move categories (physical/special/status), PP management, and move execution logic.

## Requirements

### Requirement: Move data structure
Each move SHALL be defined by a `UPokemon_Move` data asset with the following properties:

| Property | Type | Description |
|---|---|---|
| MoveName | FText | Display name of the move |
| Type | EPokemonType | The move's elemental type |
| Category | EMoveCategory | Physical, Special, or Status |
| Power | int32 | Base power (0 for status moves) |
| Accuracy | float | Hit chance (0.0-1.0, 1.0 = always hits) |
| PP | int32 | Maximum PP (Power Points) |
| CurrentPP | int32 | Remaining PP in battle |
| Priority | int32 | Speed priority (default 0, higher goes first) |
| Description | FText | Flavor text |

#### Scenario: Move categories
- GIVEN a move has Category = Physical
- THEN damage uses attacker's Attack and defender's Defense
- GIVEN a move has Category = Special
- THEN damage uses attacker's SpecialAttack and defender's SpecialDefense
- GIVEN a move has Category = Status
- THEN the move deals no direct damage (Power = 0)

### Requirement: PP management
Each move SHALL consume 1 PP when used.

#### Scenario: PP decreases on use
- GIVEN a Pokemon uses a move
- THEN decrement CurrentPP by 1
- AND when CurrentPP reaches 0, the move cannot be used

#### Scenario: PP recovery
- GIVEN a battle ends
- THEN all moves' CurrentPP SHALL be restored to their maximum PP

### Requirement: Move selection
The player SHALL only be able to select moves that have CurrentPP > 0.

#### Scenario: No usable moves
- GIVEN all of a Pokemon's moves have 0 PP
- THEN the Pokemon SHALL use "Struggle"
- AND Struggle deals recoil damage to the user

### Requirement: Accuracy check
Before a move hits, the system SHALL check accuracy.

#### Scenario: Move hits
- GIVEN a move with Accuracy 1.0
- THEN it always hits
- GIVEN a move with Accuracy < 1.0
- THEN generate a random roll
- WHEN the roll is below Accuracy
- THEN the move hits
- WHEN the roll is above Accuracy
- THEN the move misses

### Requirement: Priority moves
Some moves SHALL have priority that bypasses Speed order.

#### Scenario: Priority move goes first
- GIVEN a move has Priority > 0
- THEN the Pokemon using it SHALL act before Pokemon with lower priority moves
- WHEN both Pokemon use moves with the same priority
- THEN Speed determines order normally

## Technical Design

### New classes

```
UPokemon_Move (UDataAsset)
├── UPROPERTY: MoveName (FText)
├── UPROPERTY: Type (EPokemonType)
├── UPROPERTY: Category (EMoveCategory)
├── UPROPERTY: Power (int32)
├── UPROPERTY: Accuracy (float)
├── UPROPERTY: MaxPP (int32)
├── UPROPERTY: Priority (int32)
├── UPROPERTY: Description (FText)
└── Functions:
    ├── CanUse() → bool (CurrentPP > 0)
    ├── ConsumePP()
    └── RestorePP()
```

```
EMoveCategory (UENUM)
├── Physical
├── Special
└── Status
```

### Integration with APokemon_Pokemon

Add to `APokemon_Pokemon`:
```cpp
UPROPERTY(EditAnywhere, Category = "Pokemon|Moves")
TArray<UPokemon_Move*> MoveSet;

UFUNCTION(BlueprintCallable)
bool HasUsableMove() const;

UFUNCTION(BlueprintCallable)
UPokemon_Move* GetMove(int32 Index) const;
```

### Initial move data

Create a set of `UPokemon_Move` data assets for testing:
- Tackle (Normal, Physical, 40, 1.0, 35)
- Ember (Fire, Special, 40, 1.0, 25)
- Water Gun (Water, Special, 40, 1.0, 25)
- Vine Whip (Grass, Physical, 45, 1.0, 25)
- Thunder Shock (Electric, Special, 40, 1.0, 30)
- Quick Attack (Normal, Physical, 40, 1.0, 30, Priority=1)
