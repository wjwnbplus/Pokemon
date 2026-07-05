### New spec: type-system

+ `EPokemonType` enum with 19 types (None through Fairy)
+ Full type effectiveness chart (18×18 matrix)
+ `GetTypeEffectiveness()` function returning 0.0, 0.25, 0.5, 1.0, 2.0, or 4.0
+ STAB (Same-Type Attack Bonus): 1.5x multiplier when move type matches attacker type
+ Pokemon can have 1 or 2 types (Type1, Type2)
