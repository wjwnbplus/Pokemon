# battle-ui Specification

## Purpose
Define the battle UI layout and interaction flow using UMG Widget Blueprint. The `UBattleHUDWidget` C++ base class provides BlueprintImplementableEvent hooks that `ABattleManager` calls during battle phases.

## Requirements

### Requirement: Battle screen layout

```
┌──────────────────────────────────────┐
│                                      │
│         [NPC Pokemon Sprite]         │
│         HP Bar (NPC)                 │
│                                      │
│                                      │
│         [Player Pokemon Sprite]      │
│         HP Bar (Player)              │
│                                      │
├──────────────────────────────────────┤
│  Fight  │  Switch  │  Bag  │  Run    │  ← Main menu
├──────────────────────────────────────┤
│  Message box                         │
│  "What will Pikachu do?"             │
└──────────────────────────────────────┘
```

### Requirement: HP bars
- Show `CurrentHP / MaxHP` text
- Bar color: Green (>50%), Yellow (20-50%), Red (<20%)
- Smooth animation on HP change
- Show Pokemon name and level

### Requirement: Action menu
2×2 grid: Fight / Switch / Bag / Run

#### Scenario: Fight → Move selection
- Press Fight → show move submenu
- List up to 4 moves with PP display ("Ember PP: 24/25")
- Grey out moves with 0 PP
- Select move → `BattleManager::ExecuteMove()`

#### Scenario: Switch → Party selection
- Press Switch → show party list
- Show each Pokemon's name and HP
- Grey out fainted Pokemon
- Select → `BattleManager::PlayerSwitchPokemon()`

#### Scenario: Bag
- Press Bag → show "Not yet implemented" (deferred)

#### Scenario: Run
- Press Run → `BattleManager::PlayerAttemptRun()`

### Requirement: Message box
Display battle events sequentially. Player presses button to advance.

#### Scenario: Message flow
1. "XXX used YYY!"
2. "It's super effective!" (if applicable)
3. "Defender lost 47 HP!"
4. "Defender fainted!" (if applicable)

### Requirement: Turn indicator
- Player's turn: show action menu + "What will XXX do?"
- NPC's turn: hide menu, execute automatically

## Technical Design

### UBattleHUDWidget C++ class

```cpp
UCLASS()
class UBattleHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // Called by BattleManager
    UFUNCTION(BlueprintImplementableEvent, Category = "BattleUI")
    void ShowMainMenu();

    UFUNCTION(BlueprintImplementableEvent, Category = "BattleUI")
    void ShowMoveSelection(const TArray<UPokemon_Move*>& Moves);

    UFUNCTION(BlueprintImplementableEvent, Category = "BattleUI")
    void ShowPartySelection(const TArray<APokemon_Pokemon*>& Party);

    UFUNCTION(BlueprintImplementableEvent, Category = "BattleUI")
    void UpdatePlayerHP(float CurrentHP, float MaxHP, const FString& Name);

    UFUNCTION(BlueprintImplementableEvent, Category = "BattleUI")
    void UpdateNPCHP(float CurrentHP, float MaxHP, const FString& Name);

    UFUNCTION(BlueprintImplementableEvent, Category = "BattleUI")
    void ShowMessage(const FText& Message);

    UFUNCTION(BlueprintImplementableEvent, Category = "BattleUI")
    void ShowPokemonSprite(bool bIsPlayer, UTexture2D* Sprite);

    UFUNCTION(BlueprintImplementableEvent, Category = "BattleUI")
    void ShowVictory();

    UFUNCTION(BlueprintImplementableEvent, Category = "BattleUI")
    void ShowDefeat();

    // Button callbacks → call BattleManager
    UFUNCTION(BlueprintCallable, Category = "BattleUI")
    void OnFightSelected();

    UFUNCTION(BlueprintCallable, Category = "BattleUI")
    void OnSwitchSelected();

    UFUNCTION(BlueprintCallable, Category = "BattleUI")
    void OnBagSelected();

    UFUNCTION(BlueprintCallable, Category = "BattleUI")
    void OnRunSelected();

    UFUNCTION(BlueprintCallable, Category = "BattleUI")
    void OnMoveSelected(int32 MoveIndex);

    UFUNCTION(BlueprintCallable, Category = "BattleUI")
    void OnPokemonSelected(int32 PartyIndex);

private:
    UPROPERTY()
    ABattleManager* BattleManager;
};
```

### Widget hierarchy

```
WBP_BattleHUD (Canvas Panel)
├── NPCPokemonPanel
│   ├── NPCSprite (Image)
│   └── NPCHPBar (ProgressBar + Text)
├── PlayerPokemonPanel
│   ├── PlayerSprite (Image)
│   └── PlayerHPBar (ProgressBar + Text)
├── ActionMenuPanel (2×2 Grid)
│   ├── FightButton
│   ├── SwitchButton
│   ├── BagButton
│   └── RunButton
├── MoveSelectionPanel (hidden by default)
│   ├── MoveButton1 (Text + PP Text)
│   ├── MoveButton2
│   ├── MoveButton3
│   └── MoveButton4
├── PartySelectionPanel (hidden by default)
│   └── PartyMemberButtons[]
└── MessagePanel
    └── MessageText (TextBlock)
```

### Integration with BattleManager

```cpp
// ABattleManager creates and shows the widget:
void ABattleManager::StartBattle(...)
{
    HUDWidget = CreateWidget<UBattleHUDWidget>(GetWorld(), HUDWidgetClass);
    HUDWidget->AddToViewport();
    HUDWidget->ShowMainMenu();
    HUDWidget->ShowMessage(FText::FromString("Battle start!"));
}
```
