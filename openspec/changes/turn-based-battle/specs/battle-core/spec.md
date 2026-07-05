### Updated spec: battle-core (Plan C)

Architecture changed: no GameplayAbility used.

+ `ABattleManager` controls turn flow synchronously
+ `UGameplayEffect` (runtime Instant) carries damage parameters via SetByCaller
+ `UPokemonDamageExecution` calculates damage using attributes + type chart + STAB
+ `EBattlePhase` state machine: Init → PlayerAction → NPCAction → Resolve → Victory/Defeat
+ Move execution: accuracy → PP → create GE → apply → check faint
