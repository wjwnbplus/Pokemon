# Change: Turn-Based Battle System

## Summary
Implement a classic Pokemon turn-based battle system. This is the core gameplay feature that transforms the project from an overworld exploration prototype into a functional Pokemon battle game.

## Affected Specs
- `battle-core` — New spec (battle flow, damage formula, turn logic)
- `pokemon-moves` — New spec (move data, PP, accuracy)
- `type-system` — New spec (type enum, effectiveness chart, STAB)
- `battle-ui` — New spec (battle HUD layout, menus, messages)

## Implementation Phases

### Phase 1: Foundation (Type System + Move System)
1. Create `EPokemonType` enum in a new header `Pokemon_Types.h`
2. Implement `GetTypeEffectiveness()` function with full type chart
3. Create `EMoveCategory` enum
4. Create `UPokemon_Move` data asset class
5. Add `Type1`, `Type2`, `MoveSet` to `APokemon_Pokemon`
6. Create initial move data assets (Tackle, Ember, Water Gun, etc.)

### Phase 2: Battle Core
1. Create `EBattlePhase` enum
2. Create `ABattleManager` actor class
3. Implement `StartBattle()` — spawn manager, set references
4. Implement turn flow: PlayerAction → NPCAction → Resolve
5. Implement damage calculation formula
6. Implement accuracy check
7. Implement priority system
8. Implement faint detection and switch logic
9. Implement battle end conditions
10. Wire `APokemonPlayerController::OnInteract()` to start battle

### Phase 3: NPC AI
1. Implement NPC move selection (random from available moves)
2. Implement NPC switch logic (auto-send next Pokemon on faint)

### Phase 4: Battle UI
1. Create `UBattleHUDWidget` base class in C++
2. Create Widget Blueprint with layout (HP bars, sprites, menus)
3. Implement menu navigation (Fight → Move selection → Execute)
4. Implement HP bar animation
5. Implement message box with progression
6. Implement victory/defeat screens

## Dependencies
- Phase 1 has no dependencies
- Phase 2 depends on Phase 1 (needs types + moves)
- Phase 3 depends on Phase 2 (needs battle manager)
- Phase 4 depends on Phase 2 (needs battle state to display)

## Estimated Effort
- Phase 1: ~200 lines C++
- Phase 2: ~500 lines C++
- Phase 3: ~100 lines C++
- Phase 4: ~300 lines C++ + Widget Blueprint work
- Total: ~1100 lines C++ + UI Blueprints

## Open Questions
1. Should we implement status conditions (burn, freeze, paralysis, sleep, poison) in this change or defer to a future change?
   → Defer to future change. This change focuses on core battle flow only.
2. Should we implement held items in this change?
   → No, defer to future change.
3. Should wild Pokemon encounters be implemented alongside trainer battles?
   → No, trainer battles only. Wild encounters deferred.
