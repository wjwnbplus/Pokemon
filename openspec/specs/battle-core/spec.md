# battle-core Specification

## Purpose
Implement the classic Pokemon turn-based battle system using GAS (AttributeSet + GameplayEffect + ExecutionCalculation) for numerical computation, and a custom `ABattleManager` for turn flow control. No GameplayAbility is used — moves are data-driven via `UPokemon_Move` data assets.

## Architecture

```
UPokemon_Move (UDataAsset)      ← 技能数据：威力、属性、PP、命中率
        │
        ▼
ABattleManager (Actor)          ← 回合流程：排序、校验、命中判定、创建GE
        │
        ▼
UGameplayEffect (Instant)       ← 伤害载体：SetByCaller 注入参数
        │
        ▼
UPokemonDamageExecution (EC)    ← 伤害公式：读双方属性 + 属性克制 + STAB + 随机
        │
        ▼
UPokemon_AttributeSet           ← 属性修改：Health -= Damage
```

## Requirements

### Requirement: Battle initiation
The system SHALL start a battle when the player interacts with an `APokemon_BattleNPC` while overlapping it.

#### Scenario: Player initiates battle
- GIVEN the player is overlapping an `APokemon_BattleNPC`
- WHEN the player presses the Interact key
- THEN `APokemonPlayerController::OnInteract()` SHALL spawn `ABattleManager`
- AND `ABattleManager::StartBattle()` SHALL set references and display battle UI
- AND both sides SHALL send out their first Pokemon

### Requirement: Turn-based flow
Each turn consists of: Player selects action → NPC selects action → Sort by Speed → Execute in order → Check faint → Next turn.

#### Scenario: Standard turn
- GIVEN both sides have selected actions
- THEN sort by `(Speed + Move->Priority)` descending
- AND faster Pokemon acts first
- AND slower Pokemon acts second (if still alive)
- AND after both act, check for fainted Pokemon
- AND prompt switch if needed
- AND if neither side wiped, start new turn

#### Scenario: Speed tie
- GIVEN both Pokemon have equal Speed and equal Priority
- THEN randomly determine who acts first

### Requirement: Player actions
The player SHALL have the following action options during their turn:

#### Scenario: Fight
- GIVEN the player selects "Fight"
- THEN show the list of available moves for the current Pokemon
- WHEN the player selects a move
- THEN `ABattleManager::ExecuteMove(Attacker, Defender, Move)` is called
- AND the system checks accuracy → creates GE → applies to target → checks faint
- AND advances to the opponent's turn

#### Scenario: Switch
- GIVEN the player selects "Switch"
- THEN show the list of non-fainted Pokemon in the party
- WHEN the player selects a Pokemon
- THEN switch the current battle Pokemon
- AND the opponent gets a turn to attack

#### Scenario: Run
- GIVEN the player selects "Run"
- THEN calculate flee success based on Speed comparison
- WHEN flee succeeds → end battle
- WHEN flee fails → opponent gets a turn

### Requirement: NPC AI
The NPC SHALL automatically select actions using a simple AI.

#### Scenario: NPC selects a move
- GIVEN it is the NPC's turn
- THEN randomly select a move with `CurrentPP > 0` from the NPC's current Pokemon
- AND call `ExecuteMove()` with that move

#### Scenario: NPC switches on faint
- GIVEN the NPC's current Pokemon has fainted
- THEN auto-send the next non-fainted Pokemon in the party
- WHEN no remaining Pokemon → NPC loses

### Requirement: Move execution flow

```
ABattleManager::ExecuteMove(Attacker, Defender, Move)
│
├── 1. Accuracy check
│   └── FMath::FRand() < Move->Accuracy ? continue : show "Miss!"
│
├── 2. PP consumption
│   └── Move->CurrentPP--
│
├── 3. Create damage GE (UGameplayEffect, Instant, runtime NewObject)
│   ├── Bind ExecutionCalculation: UPokemonDamageExecution
│   └── SetByCaller: Move.Power, Move.Type, Move.Category
│
├── 4. Apply GE to target
│   └── AttackerASC->ApplyGameplayEffectSpecToTarget(Spec, DefenderASC)
│   └── GAS invokes UPokemonDamageExecution::Execute()
│       ├── Read SetByCaller → Power, Type, Category
│       ├── Read attacker attributes (Attack or SpecialAttack)
│       ├── Read defender attributes (Defense or SpecialDefense)
│       ├── Calculate: ((2*Lv/5+2)*Power*A/D/50+2)
│       ├── Apply STAB (1.5x if move type matches attacker type)
│       ├── Apply type effectiveness (0x, 0.25x, 0.5x, 1x, 2x, 4x)
│       ├── Apply random variance (0.85~1.0)
│       └── Output: Target.Health -= FinalDamage
│
└── 5. Check faint
    └── Health <= 0 ? HandleFaint(Pokemon)
```

### Requirement: Damage calculation (ExecutionCalculation)

```
Damage = ((2 * Level / 5 + 2) * Power * Attack / Defense / 50 + 2) * Modifier

Modifier = STAB × TypeEffectiveness × Random(0.85, 1.0)

Where:
  Level = 50 (fixed)
  Power = Move->Power (from SetByCaller)
  Attack = attacker's Attack (Physical) or SpecialAttack (Special)
  Defense = defender's Defense (Physical) or SpecialDefense (Special)
  STAB = 1.5 if Move.Type matches attacker's Type1 or Type2
  TypeEffectiveness = from type chart (0x ~ 4x)
```

#### Scenario: Physical vs Special
- GIVEN Move->Category == Physical
- THEN use attacker's Attack attribute and defender's Defense attribute
- GIVEN Move->Category == Special
- THEN use attacker's SpecialAttack and defender's SpecialDefense

### Requirement: Battle end conditions

#### Scenario: Player wins
- GIVEN all NPC Pokemon have fainted
- THEN display "Victory" and return to overworld

#### Scenario: Player loses
- GIVEN all player Pokemon have fainted
- THEN display "White Out" and return to overworld

### Requirement: Battle state management

`ABattleManager` SHALL track:
- `PlayerTrainer` (APokemon_BaseCharacter*)
- `NPCTrainer` (APokemon_BaseCharacter*)
- `PlayerPokemon` (APokemon_Pokemon*)
- `NPCPokemon` (APokemon_Pokemon*)
- `BattlePhase` (EBattlePhase enum)
- `bIsPlayerTurn` (bool)

`EBattlePhase` enum:
- `Init` → `PlayerAction` → `NPCAction` → `Resolve` → `Victory` or `Defeat`

`ABattleManager` SHALL be spawned when battle starts and destroyed when battle ends.

## Technical Design

### New classes

| Class | Type | Role |
|-------|------|------|
| `ABattleManager` | Actor | Turn flow, move execution, faint handling |
| `EBattlePhase` | UENUM | Phase state machine |
| `UPokemonDamageExecution` | GameplayEffectExecutionCalculation | Damage formula |
| `UBattleHUDWidget` | UUserWidget | Battle UI base class |

### Existing classes modified

| Class | Changes |
|-------|---------|
| `APokemonPlayerController::OnInteract()` | Spawn ABattleManager instead of debug message |
| `APokemon_Pokemon` | Add `PokemonName`, `Type1`, `Type2`, `MoveSet`, `bIsFainted` |
| `APokemon_BaseCharacter` | No changes (already has PokemonParty + CurrentBattlePokemon) |
| `APokemon_BattleNPC` | No code changes needed (MoveSet comes from Pokemon) |

### GAS components used

| Component | Used? | Reason |
|-----------|-------|--------|
| AttributeSet | ✅ Yes | Store HP + 6 stats |
| GameplayEffect (Instant) | ✅ Yes | Damage application carrier |
| ExecutionCalculation | ✅ Yes | Pokemon-specific damage formula |
| GameplayTag | ✅ Yes | Type tags, status tags, effect tags |
| GameplayCue | ✅ Optional | Visual hit effects (deferred) |
| GameplayAbility | ❌ No | Not needed for turn-based flow |
