# Tasks: Turn-Based Battle System

## Phase 1: Foundation (Type System + Move System)

### Task 1.1: Create EPokemonType enum and type chart
- [ ] Create `Source/Pokemon/Public/Pokemon/Pokemon_Types.h`
- [ ] Define `EPokemonType` enum with 19 types
- [ ] Implement `GetTypeEffectiveness()` with full type chart
- [ ] Create `Source/Pokemon/Private/Pokemon/Pokemon_Types.cpp`
- **Files:** `Pokemon_Types.h`, `Pokemon_Types.cpp`
- **Effort:** ~80 lines

### Task 1.2: Create EMoveCategory and UPokemon_Move
- [ ] Create `Source/Pokemon/Public/Pokemon/Pokemon_Move.h`
- [ ] Define `EMoveCategory` enum (Physical, Special, Status)
- [ ] Define `UPokemon_Move` data asset class
- [ ] Create `Source/Pokemon/Private/Pokemon/Pokemon_Move.cpp`
- **Files:** `Pokemon_Move.h`, `Pokemon_Move.cpp`
- **Effort:** ~60 lines

### Task 1.3: Update APokemon_Pokemon with types and moves
- [ ] Add `Type1`, `Type2` (EPokemonType) properties
- [ ] Add `MoveSet` (TArray<UPokemon_Move*>) property
- [ ] Add `bIsFainted` (bool) property
- [ ] Add `HasUsableMove()`, `GetMove()` functions
- **Files:** `Pokemon_Pokemon.h`, `Pokemon_Pokemon.cpp`
- **Effort:** ~40 lines

### Task 1.4: Create initial move data assets
- [ ] Create 6 basic move data assets in Content/:
  - Tackle (Normal, Physical, 40, 100%, 35)
  - Ember (Fire, Special, 40, 100%, 25)
  - Water Gun (Water, Special, 40, 100%, 25)
  - Vine Whip (Grass, Physical, 45, 100%, 25)
  - Thunder Shock (Electric, Special, 40, 100%, 30)
  - Quick Attack (Normal, Physical, 40, 100%, 30, Priority=1)
- **Effort:** Blueprint work

## Phase 2: Battle Core

### Task 2.1: Create EBattlePhase enum
- [ ] Create `Source/Pokemon/Public/Battle/BattlePhase.h`
- [ ] Define enum: None, Init, PlayerAction, NPCAction, Resolve, Victory, Defeat
- **Files:** `BattlePhase.h`
- **Effort:** ~15 lines

### Task 2.2: Create ABattleManager
- [ ] Create `Source/Pokemon/Public/Battle/BattleManager.h`
- [ ] Create `Source/Pokemon/Private/Battle/BattleManager.cpp`
- [ ] Define properties: PlayerTrainer, NPCTrainer, PlayerPokemon, NPCPokemon, BattlePhase
- [ ] Implement `StartBattle()`
- [ ] Implement `PlayerSelectMove()`
- [ ] Implement `PlayerSelectSwitch()`
- [ ] Implement `PlayerAttemptRun()`
- [ ] Implement `ExecuteTurn()` with speed/priority ordering
- [ ] Implement `CalculateDamage()` with full formula
- [ ] Implement `ApplyDamage()`
- [ ] Implement `CheckFaint()`
- [ ] Implement `EndBattle()`
- **Files:** `BattleManager.h`, `BattleManager.cpp`
- **Effort:** ~400 lines

### Task 2.3: Wire battle initiation
- [ ] Update `APokemonPlayerController::OnInteract()` to spawn ABattleManager
- [ ] Pass player trainer and NPC trainer references
- **Files:** `PokemonPlayerController.cpp`
- **Effort:** ~15 lines

## Phase 3: NPC AI

### Task 3.1: NPC move selection
- [ ] Implement NPC random move selection from available moves
- [ ] Filter out moves with 0 PP
- **Files:** `BattleManager.cpp`
- **Effort:** ~30 lines

### Task 3.2: NPC switch logic
- [ ] Auto-send next non-fainted Pokemon when current faints
- [ ] Detect NPC defeat (no remaining Pokemon)
- **Files:** `BattleManager.cpp`
- **Effort:** ~40 lines

## Phase 4: Battle UI

### Task 4.1: Create UBattleHUDWidget C++ class
- [ ] Create `Source/Pokemon/Public/Battle/UI/BattleHUDWidget.h`
- [ ] Create `Source/Pokemon/Private/Battle/UI/BattleHUDWidget.cpp`
- [ ] Define BlueprintImplementableEvent functions
- **Files:** `BattleHUDWidget.h`, `BattleHUDWidget.cpp`
- **Effort:** ~60 lines

### Task 4.2: Create Widget Blueprint
- [ ] Create WBP_BattleHUD Widget Blueprint
- [ ] Layout: NPC sprite + HP bar, Player sprite + HP bar
- [ ] Layout: Action menu (Fight/Switch/Bag/Run)
- [ ] Layout: Move selection submenu
- [ ] Layout: Party selection submenu
- [ ] Layout: Message box
- **Effort:** Blueprint work

### Task 4.3: Wire UI to BattleManager
- [ ] BattleManager creates and shows BattleHUDWidget on battle start
- [ ] BattleManager calls UI update functions during battle
- [ ] UI button callbacks call BattleManager methods
- **Files:** `BattleManager.cpp`, `BattleHUDWidget.cpp`
- **Effort:** ~80 lines
