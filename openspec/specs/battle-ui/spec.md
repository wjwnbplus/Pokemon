# battle-ui Specification

## Purpose
Define the battle UI layout and interaction flow for the turn-based battle system. The UI SHALL be implemented using UMG (Unreal Motion Graphics) with Widget Blueprints.

## Requirements

### Requirement: Battle screen layout
The battle screen SHALL display the following elements:

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
│  Fight  │  Switch  │  Bag  │  Run    │  ← Main menu (4 buttons)
├──────────────────────────────────────┤
│  Battle log / Message box            │
│  "What will Pikachu do?"             │
└──────────────────────────────────────┘
```

#### Scenario: Battle starts
- GIVEN a battle begins
- THEN display the battle screen
- AND show both Pokemon sprites
- AND show both HP bars at full
- AND show the main action menu

### Requirement: HP bars
Each HP bar SHALL display:
- Current HP / Max HP text (e.g. "100/100")
- Colored bar: Green (>50%), Yellow (20-50%), Red (<20%)
- Pokemon name and level

#### Scenario: HP decreases
- GIVEN a Pokemon takes damage
- THEN animate the HP bar decreasing smoothly
- AND update the HP text

#### Scenario: Pokemon faints
- GIVEN a Pokemon's HP reaches 0
- THEN show "XXX fainted!" message
- AND hide the fainted Pokemon's sprite

### Requirement: Action menu
The main menu SHALL have 4 buttons arranged in a 2×2 grid.

#### Scenario: Fight button
- GIVEN the player presses "Fight"
- THEN show the move selection submenu
- AND list up to 4 moves with PP display (e.g. "Ember PP: 25/25")
- AND grey out moves with 0 PP

#### Scenario: Switch button
- GIVEN the player presses "Switch"
- THEN show the party selection screen
- AND list all non-fainted Pokemon
- AND show each Pokemon's name, HP, and status
- AND grey out fainted Pokemon

#### Scenario: Bag button
- GIVEN the player presses "Bag"
- THEN show "Not implemented yet" message (future scope)

#### Scenario: Run button
- GIVEN the player presses "Run"
- THEN attempt to flee
- AND show flee result message

### Requirement: Message box
The message box SHALL display battle events one at a time.

#### Scenario: Display messages
- GIVEN a move is used
- THEN show "XXX used YYY!"
- GIVEN a move hits
- THEN show effectiveness message ("It's super effective!", "It's not very effective...", etc.)
- GIVEN a Pokemon faints
- THEN show "XXX fainted!"
- GIVEN a battle ends
- THEN show "You won!" or "You lost..."

#### Scenario: Message progression
- GIVEN there are multiple messages to display
- THEN show them one at a time
- AND the player presses a button to advance to the next message

### Requirement: Turn indicator
The UI SHALL indicate whose turn it is.

#### Scenario: Player's turn
- GIVEN it is the player's turn
- THEN show the action menu
- AND show "What will XXX do?" message

#### Scenario: NPC's turn
- GIVEN it is the NPC's turn
- THEN hide the action menu
- AND show "Wild XXX is thinking..." or just execute automatically

## Technical Design

### New classes

```
UBattleHUDWidget (UUserWidget)
├── Bindings (BlueprintImplementableEvent):
│   ├── ShowMainMenu()
│   ├── ShowMoveSelection(TArray<UPokemon_Move*> Moves)
│   ├── ShowPartySelection(TArray<APokemon_Pokemon*> Party)
│   ├── UpdatePlayerHP(float CurrentHP, float MaxHP)
│   ├── UpdateNPCHP(float CurrentHP, float MaxHP)
│   ├── ShowMessage(FText Message)
│   ├── ShowPokemonSprite(bool bIsPlayer, UTexture2D* Sprite)
│   ├── ShowFaintAnimation(bool bIsPlayer)
│   ├── ShowVictory()
│   └── ShowDefeat()
├── Input callbacks:
│   ├── OnFightSelected()
│   ├── OnSwitchSelected()
│   ├── OnBagSelected()
│   ├── OnRunSelected()
│   ├── OnMoveSelected(int32 MoveIndex)
│   └── OnPokemonSelected(int32 PartyIndex)
```

### Integration with ABattleManager

The `ABattleManager` SHALL hold a reference to the `UBattleHUDWidget` and call its functions during battle phases.

### Widget hierarchy

```
BattleHUDWidget (Canvas Panel)
├── PokemonDisplayArea
│   ├── NPCPokemonPanel
│   │   ├── NPCSprite (Image)
│   │   └── NPCHPBar (ProgressBar + Text)
│   └── PlayerPokemonPanel
│       ├── PlayerSprite (Image)
│       └── PlayerHPBar (ProgressBar + Text)
├── ActionMenuPanel
│   ├── FightButton (Button)
│   ├── SwitchButton (Button)
│   ├── BagButton (Button)
│   └── RunButton (Button)
├── MoveSelectionPanel (hidden by default)
│   ├── MoveButton1 (Button + PP Text)
│   ├── MoveButton2 (Button + PP Text)
│   ├── MoveButton3 (Button + PP Text)
│   └── MoveButton4 (Button + PP Text)
├── PartySelectionPanel (hidden by default)
│   └── PartyMemberButtons[]
└── MessagePanel
    └── MessageText (Text Block)
```
