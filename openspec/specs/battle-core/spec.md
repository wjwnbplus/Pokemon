# battle-core Specification

## Purpose
Implement the classic Pokemon turn-based battle system. This is the core gameplay loop where the player's Pokemon fights against an NPC trainer's Pokemon in a turn-by-turn fashion.

## Requirements

### Requirement: Battle initiation
The system SHALL start a battle when the player interacts with an `APokemon_BattleNPC` while overlapping it.

#### Scenario: Player initiates battle
- GIVEN the player is overlapping an `APokemon_BattleNPC`
- WHEN the player presses the Interact key
- THEN the system SHALL transition from overworld to battle mode
- AND the battle UI SHALL be displayed
- AND both sides SHALL send out their first Pokemon

#### Scenario: NPC has no Pokemon
- GIVEN the player initiates a battle with an NPC
- WHEN the NPC's PokemonParty is empty
- THEN the battle SHALL NOT start
- AND a message SHALL indicate the NPC has no Pokemon

### Requirement: Turn-based flow
The battle SHALL follow a classic turn-based flow: each side selects an action, then actions resolve in order of Pokemon Speed (higher goes first).

#### Scenario: Standard turn flow
- GIVEN a battle is active
- WHEN both sides have selected their actions
- THEN the faster Pokemon SHALL act first
- AND the slower Pokemon SHALL act second
- AND after both act, check for fainted Pokemon
- AND if neither side has fainted, start a new turn
- AND if one side has all fainted Pokemon, end the battle

#### Scenario: Speed tie
- GIVEN both Pokemon have equal Speed
- WHEN determining turn order
- THEN the system SHALL randomly choose who goes first

### Requirement: Player actions
The player SHALL have the following action options during their turn:

1. **Fight** — Select a move from the current Pokemon's move set
2. **Switch** — Switch to another Pokemon in the party
3. **Bag** — Use an item (future scope)
4. **Run** — Attempt to flee from battle

#### Scenario: Fight action
- GIVEN the player selects "Fight"
- THEN SHOW the list of available moves for the current Pokemon
- WHEN the player selects a move
- THEN execute the move against the opponent's Pokemon
- AND advance to the opponent's turn

#### Scenario: Switch action
- GIVEN the player selects "Switch"
- THEN SHOW the list of non-fainted Pokemon in the party
- WHEN the player selects a Pokemon
- THEN switch the current battle Pokemon with the selected one
- AND the opponent gets a free turn to attack

#### Scenario: Run action
- GIVEN the player selects "Run"
- THEN calculate flee success based on Speed comparison
- WHEN flee succeeds
- THEN end the battle
- WHEN flee fails
- THEN the opponent gets a turn

### Requirement: NPC AI actions
The NPC SHALL automatically select actions using a simple AI.

#### Scenario: NPC selects a move
- GIVEN it is the NPC's turn
- THEN the NPC SHALL randomly select a move from its current Pokemon's move set
- AND execute it against the player's Pokemon

#### Scenario: NPC switches Pokemon
- GIVEN the NPC's current Pokemon has fainted
- THEN the NPC SHALL automatically send out the next non-fainted Pokemon in its party
- WHEN the NPC has no remaining Pokemon
- THEN the NPC loses the battle

### Requirement: Battle end conditions
The battle SHALL end when one trainer has no usable Pokemon remaining.

#### Scenario: Player wins
- GIVEN all of the NPC's Pokemon have fainted
- THEN display "Victory" message
- AND return to overworld mode

#### Scenario: Player loses (white-out)
- GIVEN all of the player's Pokemon have fainted
- THEN display "White Out" message
- AND return to overworld mode at the last visited Pokemon Center

#### Scenario: Player flees
- GIVEN the player successfully flees
- THEN end the battle
- AND return to overworld mode

### Requirement: Battle state management
The system SHALL manage battle state through a dedicated `ABattleManager` actor.

- The `ABattleManager` SHALL track:
  - Player trainer reference
  - NPC trainer reference
  - Player's current Pokemon
  - NPC's current Pokemon
  - Turn order
  - Battle phase (Init, PlayerAction, NPCAction, Resolve, Victory, Defeat)
- The `ABattleManager` SHALL be spawned when a battle starts
- The `ABattleManager` SHALL be destroyed when the battle ends

### Requirement: Damage calculation
Damage SHALL be calculated using the classic Pokemon formula:

```
Damage = ((2 * Level / 5 + 2) * Power * A/D / 50 + 2) * Modifier
```

Where:
- `Level` = attacker's level (fixed at 50)
- `Power` = move's base power
- `A` = attacker's Attack (for physical moves) or SpecialAttack (for special moves)
- `D` = defender's Defense (for physical moves) or SpecialDefense (for special moves)
- `Modifier` = STAB (1.5 if move type matches attacker type) × Type effectiveness × Random (0.85-1.0)

#### Scenario: Physical move damage
- GIVEN a Pokemon uses a physical move
- THEN use the attacker's Attack attribute and defender's Defense attribute in the formula

#### Scenario: Special move damage
- GIVEN a Pokemon uses a special move
- THEN use the attacker's SpecialAttack attribute and defender's SpecialDefense attribute in the formula

#### Scenario: STAB bonus
- GIVEN the move's type matches one of the attacker's types
- THEN apply a 1.5x damage multiplier

#### Scenario: Type effectiveness
- GIVEN the move's type has an effectiveness against the defender's type(s)
- THEN apply the corresponding multiplier (0x, 0.25x, 0.5x, 2x, 4x)

#### Scenario: Random variance
- GIVEN damage is calculated
- THEN multiply by a random factor between 0.85 and 1.0

### Requirement: Fainting
A Pokemon SHALL faint when its Health reaches 0.

#### Scenario: Pokemon faints
- GIVEN a Pokemon's Health reaches 0 after damage
- THEN set the Pokemon's Health to 0
- AND mark the Pokemon as fainted
- AND the trainer must send out their next Pokemon

## Technical Design

### New classes needed

```
ABattleManager (Actor)
├── Properties:
│   ├── PlayerTrainer (APokemon_BaseCharacter*)
│   ├── NPCTrainer (APokemon_BaseCharacter*)
│   ├── PlayerPokemon (APokemon_Pokemon*)
│   ├── NPCPokemon (APokemon_Pokemon*)
│   ├── BattlePhase (EBattlePhase enum)
│   └── bIsPlayerTurn (bool)
├── Functions:
│   ├── StartBattle(Player, NPC)
│   ├── PlayerSelectMove(MoveIndex)
│   ├── PlayerSelectSwitch(PokemonIndex)
│   ├── PlayerAttemptRun()
│   ├── ExecuteTurn()
│   ├── CalculateDamage(Attacker, Defender, Move)
│   ├── ApplyDamage(Target, Damage)
│   ├── CheckFaint(Pokemon)
│   ├── EndBattle(Winner)
│   └── GetSpeedOrder(A, B) → returns first actor
```

```
EBattlePhase (UENUM)
├── None
├── Init
├── PlayerAction
├── NPCAction
├── Resolve
├── Victory
└── Defeat
```

### Integration points

| Existing class | Changes needed |
|---|---|
| `APokemonPlayerController::OnInteract()` | Call `ABattleManager::StartBattle()` instead of debug message |
| `APokemon_Pokemon` | Add `FName PokemonName`, `TArray<UPokemon_Move*> MoveSet`, `EPokemonType Type1/Type2`, `bool bIsFainted` |
| `APokemon_BaseCharacter` | No changes needed (already has PokemonParty + CurrentBattlePokemon) |
| `APokemon_BattleNPC` | Add `TArray<UPokemon_Move*> MoveSet` for NPC AI selection |

### Dependencies

- Requires `pokemon-moves` spec for move data
- Requires `type-system` spec for type effectiveness
- Requires `battle-ui` spec for UI display
