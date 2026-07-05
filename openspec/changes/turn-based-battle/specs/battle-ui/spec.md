### Updated spec: battle-ui (Plan C)

+ `UBattleHUDWidget` C++ base class with BlueprintImplementableEvent methods
+ Layout: NPC/Player sprite + HP bar, action menu (2×2), message box
+ HP bars: Green (>50%), Yellow (20-50%), Red (<20%)
+ Move selection submenu with PP display
+ Party selection submenu
+ `BattleManager` calls widget methods to update UI during battle
+ Widget button callbacks call `BattleManager` methods
