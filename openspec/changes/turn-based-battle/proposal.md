# Change: Turn-Based Battle System (Plan C)

## Summary
Implement a classic Pokemon turn-based battle system using the **Plan C** architecture:
- `UPokemon_Move` (UDataAsset) — pure data for skills
- `ABattleManager` (Actor) — turn flow and move execution
- `UGameplayEffect` + `UPokemonDamageExecution` — damage calculation via GAS
- No GameplayAbility used

## Architecture Overview

```
UPokemon_Move (DataAsset)        ABattleManager (Actor)
├── Power, Type, Category        ├── StartBattle()
├── Accuracy, PP, Priority       ├── ExecuteMove()
└── CanUse() / RestorePP()       ├── HandleFaint()
        │                        └── EndBattle()
        │                              │
        ▼                              │
BattleManager::ExecuteMove()           │
├── Accuracy check                    │
├── PP consumption                    │
├── Create GE (runtime)               │
│   ├── Bind ExecutionCalculation     │
│   └── SetByCaller: Power/Type/Cat   │
├── ApplyGEToTarget                   │
│   └── ExecutionCalc::Execute()      │
│       ├── Read SetByCaller          │
│       ├── Read AttributeSet         │
│       ├── Damage formula            │
│       ├── STAB + Type effectiveness │
│       └── Output: Health -= Damage  │
└── Check faint                       │
```

## Affected Specs
- `battle-core` — Battle flow, damage formula, turn logic (rewritten for Plan C)
- `pokemon-moves` — Move data assets (rewritten, no GA)
- `type-system` — Type enum, effectiveness chart, STAB (updated)
- `battle-ui` — Battle HUD layout (updated)

## Implementation Phases

### Phase 1: Data Layer (Type System + Move Data)
1. Create `EPokemonType` enum + `GetTypeEffectiveness()` + `GetSTABMultiplier()`
2. Create `EMoveCategory` enum
3. Create `UPokemon_Move` UDataAsset class
4. Add `Type1`, `Type2`, `MoveSet`, `PokemonName` to `APokemon_Pokemon`
5. Create 6 initial move data assets (Tackle, Ember, Water Gun, etc.)

### Phase 2: Battle Core
1. Create `EBattlePhase` enum
2. Create `ABattleManager` actor (turn flow, move execution, faint handling)
3. Create `UPokemonDamageExecution` (damage formula with type chart + STAB)
4. Wire `APokemonPlayerController::OnInteract()` to spawn BattleManager
5. Implement NPC AI (random move selection, auto-switch on faint)

### Phase 3: Battle UI
1. Create `UBattleHUDWidget` C++ base class
2. Create Widget Blueprint (HP bars, menus, messages)
3. Wire UI to BattleManager callbacks

## Dependencies
- Phase 1 has no dependencies
- Phase 2 depends on Phase 1 (needs types + moves)
- Phase 3 depends on Phase 2 (needs BattleManager for callbacks)

## GAS Components Used

| Component | Used? | Reason |
|-----------|-------|--------|
| AttributeSet | ✅ | Store HP + 6 stats |
| GameplayEffect (Instant) | ✅ | Damage application carrier |
| ExecutionCalculation | ✅ | Pokemon damage formula |
| GameplayTag | ✅ | Type/status/effect tags |
| GameplayCue | ✅ Optional | Hit effects (deferred) |
| GameplayAbility | ❌ | Not needed for turn-based |

## Open Questions
1. Status conditions (burn, freeze, etc.) — deferred to future change
2. Held items — deferred to future change
3. Wild Pokemon encounters — deferred to future change
4. GA extension point — `UPokemon_Move` can optionally reference a GA class for complex moves (not implemented now)
