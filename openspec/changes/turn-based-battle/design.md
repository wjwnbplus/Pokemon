# Design: Turn-Based Battle System

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                     ABattleManager                           │
│  ┌─────────────┐  ┌──────────────┐  ┌──────────────────┐   │
│  │ Battle Flow │  │ Damage Calc  │  │ Turn Resolution  │   │
│  │ StateMachine│  │ + Type Chart │  │ + Faint Check    │   │
│  └──────┬──────┘  └──────┬───────┘  └────────┬─────────┘   │
│         │                │                    │             │
│         └────────────────┼────────────────────┘             │
│                          │                                  │
│  ┌───────────────────────┴──────────────────────────────┐   │
│  │              References                               │   │
│  │  PlayerTrainer  │  NPCTrainer  │  BattleHUDWidget    │   │
│  │  PlayerPokemon  │  NPCPokemon  │                     │   │
│  └──────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
         │                        │
         ▼                        ▼
┌─────────────────┐    ┌──────────────────────┐
│  APokemon_Pokemon│    │  UBattleHUDWidget    │
│  - Type1/Type2   │    │  (UserWidget)        │
│  - MoveSet[]     │    │  - HP Bars           │
│  - bIsFainted    │    │  - Action Menu       │
│  - GAS Attributes│    │  - Message Box       │
└─────────────────┘    └──────────────────────┘
```

## Key Design Decisions

### 1. ABattleManager as central controller
A dedicated Actor manages all battle state. This keeps battle logic separate from the overworld character controllers. The manager is spawned when battle starts and destroyed when it ends.

### 2. State machine for battle phases
The battle flow uses a simple enum-based state machine:

```
Init → PlayerAction → NPCAction → Resolve → (loop or Victory/Defeat)
```

Each phase is a method on `ABattleManager`. The `Tick()` function drives phase transitions.

### 3. Damage formula as standalone function
`CalculateDamage()` is a static utility function that takes attacker, defender, and move references. This makes it testable and reusable.

### 4. UI via UMG Widget
The battle HUD is a `UUserWidget` subclass with BlueprintImplementableEvent functions. The C++ side calls these events, and the Widget Blueprint handles the visual layout. This allows artists to modify the UI without touching C++.

## File Structure

```
Source/Pokemon/Public/
├── Pokemon/
│   ├── Pokemon_Types.h          ← EPokemonType enum + GetTypeEffectiveness()
│   ├── Pokemon_Move.h           ← UPokemon_Move data asset
│   └── Pokemon_Pokemon.h        ← Updated with Type1/Type2/MoveSet
├── Battle/
│   ├── BattleManager.h          ← ABattleManager
│   ├── BattlePhase.h            ← EBattlePhase enum
│   └── UI/
│       └── BattleHUDWidget.h    ← UBattleHUDWidget

Source/Pokemon/Private/
├── Pokemon/
│   ├── Pokemon_Types.cpp
│   ├── Pokemon_Move.cpp
│   └── Pokemon_Pokemon.cpp     ← Updated
├── Battle/
│   ├── BattleManager.cpp
│   └── UI/
│       └── BattleHUDWidget.cpp
```

## Data Flow

```
Player presses Interact
  → APokemonPlayerController::OnInteract()
    → Spawn ABattleManager
      → BattleManager::StartBattle(PlayerTrainer, NPCTrainer)
        → Set PlayerPokemon = PlayerTrainer->GetCurrentBattlePokemon()
        → Set NPCPokemon = NPCTrainer->GetCurrentBattlePokemon()
        → Create and show BattleHUDWidget
        → Set phase = PlayerAction

Player selects a move
  → BattleManager::PlayerSelectMove(MoveIndex)
    → Store selected move
    → Set phase = NPCAction

NPC selects a move (auto)
  → BattleManager::ExecuteNPCAction()
    → Randomly select from NPCPokemon->MoveSet
    → Set phase = Resolve

Resolve turn
  → BattleManager::ExecuteTurn()
    → Determine order by Speed (or Priority)
    → Execute first move (CalculateDamage → ApplyDamage → CheckFaint)
    → Execute second move (if first didn't end battle)
    → Check faint → prompt switch if needed
    → If battle continues, set phase = PlayerAction
    → If battle ends, set phase = Victory or Defeat
```
