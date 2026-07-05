### New spec: battle-core

The system SHALL now have a complete turn-based battle system with:

+ `ABattleManager` actor to manage battle state and flow
+ Turn-based execution ordered by Speed (with priority support)
+ Classic Pokemon damage formula: `((2 * Level / 5 + 2) * Power * A/D / 50 + 2) * Modifier`
+ Player actions: Fight, Switch, Run
+ NPC AI: random move selection, auto-switch on faint
+ Battle end conditions: all Pokemon fainted on either side
