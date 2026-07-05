# Tasks: Turn-Based Battle System (Plan C)

## Phase 1: Data Layer

### Task 1.1: Create EPokemonType + type chart
- [ ] Create `Source/Pokemon/Public/Pokemon/Pokemon_Types.h`
- [ ] Define `EPokemonType` enum (19 types)
- [ ] Implement `GetTypeEffectiveness()` with full 18×18 chart
- [ ] Implement `GetSTABMultiplier()`
- [ ] Create `Source/Pokemon/Private/Pokemon/Pokemon_Types.cpp`
- **Files:** `Pokemon_Types.h`, `Pokemon_Types.cpp`
- **Effort:** ~120 lines C++

### Task 1.2: Create UPokemon_Move + EMoveCategory
- [ ] Create `Source/Pokemon/Public/Pokemon/Pokemon_Move.h`
- [ ] Define `EMoveCategory` enum
- [ ] Define `UPokemon_Move` UDataAsset with all properties
- [ ] Implement `CanUse()`, `RestorePP()`
- [ ] Create `Source/Pokemon/Private/Pokemon/Pokemon_Move.cpp`
- **Files:** `Pokemon_Move.h`, `Pokemon_Move.cpp`
- **Effort:** ~60 lines C++

### Task 1.3: Update APokemon_Pokemon
- [ ] Add `Type1`, `Type2` (EPokemonType) properties
- [ ] Add `MoveSet` (TArray<UPokemon_Move*>) property
- [ ] Add `PokemonName` (FName) property
- [ ] Add `bIsFainted` (bool) property
- **Files:** `Pokemon_Pokemon.h` only (no .cpp changes needed)
- **Effort:** ~20 lines C++

### Task 1.4: Create initial move data assets
- [ ] Create `Content/Pokemon/Moves/` folder in editor
- [ ] Create 6 data assets: Tackle, Ember, Water Gun, Vine Whip, Thunder Shock, Quick Attack
- **Effort:** Blueprint work

## Phase 2: Battle Core

### Task 2.1: Create EBattlePhase enum
- [ ] Create `Source/Pokemon/Public/Battle/BattlePhase.h`
- [ ] Define: None, Init, PlayerAction, NPCAction, Resolve, Victory, Defeat
- **Files:** `BattlePhase.h`
- **Effort:** ~10 lines

### Task 2.2: Create UPokemonDamageExecution
- [ ] Create `Source/Pokemon/Public/Battle/PokemonDamageExecution.h`
- [ ] Create `Source/Pokemon/Private/Battle/PokemonDamageExecution.cpp`
- [ ] Read SetByCaller: Power, Type, Category
- [ ] Read Source attributes (Attack or SpecialAttack based on Category)
- [ ] Read Target attributes (Defense or SpecialDefense based on Category)
- [ ] Implement damage formula: `((2*50/5+2)*Power*A/D/50+2)`
- [ ] Apply STAB multiplier via `GetSTABMultiplier()`
- [ ] Apply type effectiveness via `GetTypeEffectiveness()`
- [ ] Apply random variance (0.85~1.0)
- [ ] Output: Target.Health -= FinalDamage
- **Files:** `PokemonDamageExecution.h`, `PokemonDamageExecution.cpp`
- **Effort:** ~120 lines C++

### Task 2.3: Create ABattleManager
- [ ] Create `Source/Pokemon/Public/Battle/BattleManager.h`
- [ ] Create `Source/Pokemon/Private/Battle/BattleManager.cpp`
- [ ] Properties: PlayerTrainer, NPCTrainer, PlayerPokemon, NPCPokemon, BattlePhase
- [ ] `StartBattle()` — set references, send out first Pokemon, create HUD
- [ ] `ExecuteMove(Attacker, Defender, Move)` — accuracy check, PP, create GE, apply, check faint
- [ ] `HandleFaint(Pokemon)` — prompt switch, check defeat
- [ ] `PlayerSelectMove(MoveIndex)` / `PlayerSelectSwitch(PartyIndex)` / `PlayerAttemptRun()`
- [ ] `ExecuteNPCAction()` — random move selection
- [ ] `ExecuteTurn()` — sort by Speed+Priority, execute in order
- [ ] `EndBattle(Winner)` — show result, destroy manager
- **Files:** `BattleManager.h`, `BattleManager.cpp`
- **Effort:** ~350 lines C++

### Task 2.4: Wire battle initiation
- [ ] Update `APokemonPlayerController::OnInteract()` to spawn ABattleManager
- [ ] Pass player trainer and NPC trainer references
- **Files:** `PokemonPlayerController.cpp`
- **Effort:** ~15 lines

## Phase 3: Battle UI

### Task 3.1: Create UBattleHUDWidget C++ class
- [ ] Create `Source/Pokemon/Public/Battle/UI/BattleHUDWidget.h`
- [ ] Create `Source/Pokemon/Private/Battle/UI/BattleHUDWidget.cpp`
- [ ] Define BlueprintImplementableEvent functions: ShowMainMenu, ShowMoveSelection, UpdateHP, ShowMessage, etc.
- [ ] Define BlueprintCallable button callbacks: OnFightSelected, OnMoveSelected, etc.
- **Files:** `BattleHUDWidget.h`, `BattleHUDWidget.cpp`
- **Effort:** ~60 lines C++

### Task 3.2: Create Widget Blueprint
- [ ] Create WBP_BattleHUD based on UBattleHUDWidget
- [ ] Layout: NPC sprite + HP bar, Player sprite + HP bar
- [ ] Layout: Action menu (Fight/Switch/Bag/Run)
- [ ] Layout: Move selection submenu (4 buttons + PP labels)
- [ ] Layout: Party selection submenu
- [ ] Layout: Message box
- [ ] Implement HP bar color logic (Green/Yellow/Red)
- [ ] Implement HP bar animation on damage
- **Effort:** Blueprint work

### Task 3.3: Wire UI to BattleManager
- [ ] BattleManager creates and shows BattleHUDWidget on StartBattle()
- [ ] BattleManager calls UpdateHP() after damage
- [ ] BattleManager calls ShowMessage() for battle events
- [ ] UI button callbacks call BattleManager methods
- **Files:** `BattleManager.cpp`, `BattleHUDWidget.cpp`
- **Effort:** ~50 lines C++

## Summary

| Phase | Tasks | C++ Lines | Blueprint |
|-------|-------|-----------|-----------|
| Phase 1 (Data) | 4 | ~200 | Move assets |
| Phase 2 (Core) | 4 | ~495 | — |
| Phase 3 (UI) | 3 | ~110 | WBP_BattleHUD |
| **Total** | **11** | **~805** | **Widget + Data assets** |
