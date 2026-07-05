### Updated spec: pokemon-moves (Plan C)

+ `UPokemon_Move` is a pure UDataAsset — no GA dependency
+ `EMoveCategory`: Physical, Special, Status
+ Properties: Power, Accuracy, MaxPP, Priority, Type, Category
+ `BattleManager::ExecuteMove()` reads Move data → creates GE → applies to target
+ PP management: decrement on use, restore on battle end
+ Accuracy check: `FMath::FRand() < Move->Accuracy`
+ Priority: overrides Speed for turn order
+ New move = new data asset, zero C++ code
