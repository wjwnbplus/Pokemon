# Design: Turn-Based Battle System (Plan C)

## Architecture: Three-Layer Separation

```
┌─────────────────────────────────────────────────────────────┐
│                     DATA LAYER                               │
│  UPokemon_Move (UDataAsset)    EPokemonType (enum)          │
│  ─ Describes WHAT              ─ Describes TYPES            │
│  Power, Type, Accuracy, PP     Fire, Water, Grass...        │
└──────────────────────────┬──────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                     FLOW LAYER                               │
│  ABattleManager (Actor)                                     │
│  ─ Controls WHEN and HOW                                    │
│  StartBattle, ExecuteMove, HandleFaint, EndBattle           │
│  Accuracy check, PP management, turn ordering               │
└──────────────────────────┬──────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                     COMPUTE LAYER (GAS)                      │
│  UGameplayEffect (Instant) + UPokemonDamageExecution        │
│  ─ Calculates RESULT                                        │
│  Damage formula, STAB, type effectiveness, random variance  │
│  Output → UPokemon_AttributeSet.Health -= Damage            │
└─────────────────────────────────────────────────────────────┘
```

## Key Design Decisions

### 1. No GameplayAbility
GA is designed for real-time action games (input-driven activation, animation cancellation, cooldown prediction). Turn-based Pokemon needs none of these. The trade-off:

| GA feature | Pokemon need? | Alternative |
|------------|:---:|------|
| Input-driven activation | ❌ | BattleManager calls ExecuteMove() synchronously |
| Animation cancellation | ❌ | Turn-based = sequential |
| Cooldown management | ❌ | PP system replaces it |
| Network prediction | ❌ | Turn-based = authoritative server |
| CanActivateAbility checks | ⚠️ | Data-driven conditions on UPokemon_Move |
| Automatic state cleanup | ⚠️ | GE Duration Policy handles temporary states |

### 2. GE is a runtime carrier, not a data asset
Each `UGameplayEffect` is created via `NewObject` at move execution time. It binds the `UPokemonDamageExecution` and passes move parameters via `SetByCaller`. This means:
- No need to pre-configure GE assets for each move
- One ExecutionCalculation handles all damage moves
- Move data lives in UPokemon_Move data assets only

### 3. SetByCaller for parameter passing
Move parameters (Power, Type, Category) are injected into the GE via `SetByCallerMagnitude`:
```cpp
Spec.Data->SetSetByCallerMagnitude(Tag_MovePower, Move->Power);
Spec.Data->SetSetByCallerMagnitude(Tag_MoveType, (float)Move->Type);
Spec.Data->SetSetByCallerMagnitude(Tag_MoveCategory, (float)Move->Category);
```
The ExecutionCalculation reads them back:
```cpp
float Power = Spec.GetSetByCallerMagnitude(Tag_MovePower);
EPokemonType Type = (EPokemonType)(int32)Spec.GetSetByCallerMagnitude(Tag_MoveType);
```

### 4. BattleManager is the single source of truth
All battle state lives in ABattleManager:
- Which trainers are fighting
- Which Pokemon are active
- Current phase (Init/PlayerAction/NPCAction/Resolve/Victory/Defeat)
- Turn order

BattleManager is spawned on battle start, destroyed on battle end. No persistent state leaks.

## Data Flow: One Move Execution

```
Player selects "Ember"
  │
  ▼
ABattleManager::ExecuteMove(PlayerPokemon, NPCPokemon, EmberMove)
  │
  ├── 1. CanUse? (PP > 0) ✅
  ├── 2. Accuracy (FMath::FRand() < 1.0) ✅
  ├── 3. EmberMove->CurrentPP-- (25→24)
  ├── 4. Create GE + SetByCaller(Power=40, Type=Fire, Category=Special)
  ├── 5. ApplyGEToTarget(DefenderASC)
  │     └── UPokemonDamageExecution::Execute()
  │           ├── Read SetByCaller → Power=40, Type=Fire, Cat=Special
  │           ├── Read Attacker.SpecialAttack = 120
  │           ├── Read Defender.SpecialDefense = 80
  │           ├── Dmg = ((2*50/5+2)*40*120/80/50+2) = 47
  │           ├── STAB (Fire attacker, Fire move) → 47 * 1.5 = 70
  │           ├── Type (Fire vs Grass) → 70 * 2.0 = 140
  │           ├── Random (0.85~1.0) → 140 * 0.9 = 126
  │           └── Output: Defender.Health -= 126
  ├── 6. Health <= 0? → HandleFaint()
  └── 7. Continue turn flow
```

## File Structure

```
Source/Pokemon/Public/
├── Pokemon/
│   ├── Pokemon_Types.h              ← EPokemonType enum + GetTypeEffectiveness()
│   ├── Pokemon_Move.h               ← UPokemon_Move UDataAsset + EMoveCategory
│   └── Pokemon_Pokemon.h            ← Updated: Type1, Type2, MoveSet, PokemonName
├── Battle/
│   ├── BattleManager.h              ← ABattleManager (turn flow)
│   ├── BattlePhase.h                ← EBattlePhase enum
│   ├── PokemonDamageExecution.h     ← UPokemonDamageExecution (EC)
│   └── UI/
│       └── BattleHUDWidget.h        ← UBattleHUDWidget

Source/Pokemon/Private/
├── Pokemon/
│   ├── Pokemon_Types.cpp
│   └── Pokemon_Move.cpp
├── Battle/
│   ├── BattleManager.cpp
│   ├── PokemonDamageExecution.cpp
│   └── UI/
│       └── BattleHUDWidget.cpp
```

## Extension Point: Future GA Support

If a move needs complex logic that data-driven approach can't handle, `UPokemon_Move` can optionally reference a GA:

```cpp
// Future addition — not implemented now:
UPROPERTY(EditAnywhere, Category = "Move|Advanced")
TSubclassOf<UGameplayAbility> CustomAbilityClass;
```

If set, `BattleManager::ExecuteMove()` delegates to `TryActivateAbilityByClass()` instead of the standard GE flow. This is intentionally deferred.
