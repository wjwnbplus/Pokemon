### Updated spec: type-system (Plan C)

+ `EPokemonType` enum: 19 types (None through Fairy)
+ `GetTypeEffectiveness(MoveType, DefType1, DefType2)` → 0.0~4.0 multiplier
+ `GetSTABMultiplier(MoveType, AtkType1, AtkType2)` → 1.0 or 1.5
+ Static 2D array for type chart
+ Used by `UPokemonDamageExecution::Execute()` during damage calculation
