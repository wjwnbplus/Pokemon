# pokemon-moves Specification

## Purpose
Define the Pokemon move (skill) system as pure data assets. Each move is a `UPokemon_Move` UDataAsset. No GameplayAbility is involved — the `ABattleManager` reads move data and creates `UGameplayEffect` instances at runtime to execute damage.

## Architecture

```
Content/Moves/Tackle.uasset     ← UPokemon_Move data asset (designer creates in editor)
Content/Moves/Ember.uasset      ← UPokemon_Move data asset
Content/Moves/WaterGun.uasset   ← UPokemon_Move data asset
...
        │
        ▼
ABattleManager::ExecuteMove()   ← Reads Move data → creates GE → applies to target
```

## Requirements

### Requirement: Move data structure
Each move SHALL be a `UPokemon_Move` data asset with the following properties:

| Property | Type | Description |
|---|---|---|
| MoveName | FText | Display name |
| Type | EPokemonType | Elemental type |
| Category | EMoveCategory | Physical / Special / Status |
| Power | int32 | Base power (0 = status move) |
| Accuracy | float | 0.0 ~ 1.0 (1.0 = always hits) |
| MaxPP | int32 | Maximum PP |
| Priority | int32 | Speed priority (0 = normal, higher goes first) |
| Description | FText | Flavor text |

#### Scenario: Move categories
- Physical → uses Attack vs Defense in damage formula
- Special → uses SpecialAttack vs SpecialDefense in damage formula
- Status → Power = 0, no damage GE created, handled by BattleManager

### Requirement: PP management

#### Scenario: PP consumption
- GIVEN a Pokemon uses a move
- THEN `Move->CurrentPP` decrements by 1
- AND when `CurrentPP == 0`, the move is greyed out in UI and cannot be selected

#### Scenario: PP restoration
- GIVEN a battle ends
- THEN all moves' `CurrentPP` SHALL be restored to `MaxPP`

### Requirement: Move selection
The player SHALL only see moves with `CurrentPP > 0` in the fight menu.

#### Scenario: No usable moves (Struggle)
- GIVEN all moves have `CurrentPP == 0`
- THEN the Pokemon SHALL use "Struggle" (a hardcoded fallback move with recoil)

### Requirement: Accuracy check
Before damage is applied, the system SHALL check accuracy.

#### Scenario: Accuracy check
- `Accuracy == 1.0` → always hits
- `Accuracy < 1.0` → `FMath::FRand() < Accuracy` determines hit
- Miss → show "Miss!" message, no GE created

### Requirement: Priority
Priority SHALL override Speed for turn order.

#### Scenario: Priority ordering
- Higher `Priority` value acts first
- Same priority → Speed determines order
- Example: Quick Attack (Priority=1) acts before Tackle (Priority=0) regardless of Speed

## Technical Design

### UPokemon_Move class

```cpp
// Pokemon_Move.h
UCLASS(BlueprintType)
class UPokemon_Move : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move|Info")
    FText MoveName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move|Info")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move|Battle")
    EPokemonType Type = EPokemonType::Normal;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move|Battle")
    EMoveCategory Category = EMoveCategory::Physical;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move|Battle")
    int32 Power = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move|Battle")
    float Accuracy = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move|Battle")
    int32 MaxPP = 35;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move|Battle")
    int32 Priority = 0;

    // Runtime state (not saved in data asset)
    UPROPERTY(Transient)
    int32 CurrentPP = 0;

    // Utility
    UFUNCTION(BlueprintCallable)
    bool CanUse() const { return CurrentPP > 0; }

    UFUNCTION(BlueprintCallable)
    void RestorePP() { CurrentPP = MaxPP; }
};
```

### EMoveCategory enum

```cpp
UENUM(BlueprintType)
enum class EMoveCategory : uint8
{
    Physical,
    Special,
    Status
};
```

### Integration with APokemon_Pokemon

```cpp
// Added to APokemon_Pokemon:
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon|Moves")
TArray<UPokemon_Move*> MoveSet;

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon|Identity")
FName PokemonName;
```

### How BattleManager uses Move data

```cpp
void ABattleManager::ExecuteMove(APokemon_Pokemon* Attacker, APokemon_Pokemon* Defender, UPokemon_Move* Move)
{
    // 1. Check PP
    if (!Move->CanUse()) return;

    // 2. Accuracy check
    if (FMath::FRand() > Move->Accuracy) { /* miss */ return; }

    // 3. Consume PP
    Move->CurrentPP--;

    // 4. Status moves — no GE needed
    if (Move->Category == EMoveCategory::Status) { /* handle separately */ return; }

    // 5. Create GE
    UGameplayEffect* DamageGE = NewObject<UGameplayEffect>(GetTransientPackage());
    DamageGE->DurationPolicy = EGameplayEffectDurationType::Instant;
    DamageGE->Executions[0].CalculationClass = UPokemonDamageExecution::StaticClass();

    FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(DamageGE, 1, Attacker->GetAbilitySystemComponent());
    Spec.Data->SetSetByCallerMagnitude(Tag_MovePower, Move->Power);
    Spec.Data->SetSetByCallerMagnitude(Tag_MoveType, (float)Move->Type);
    Spec.Data->SetSetByCallerMagnitude(Tag_MoveCategory, (float)Move->Category);

    // 6. Apply
    Attacker->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*Spec.Data, Defender->GetAbilitySystemComponent());

    // 7. Check faint
    if (Defender->GetAttributeSet()->GetHealth() <= 0) { HandleFaint(Defender); }
}
```

### Initial move data assets

Create in `Content/Pokemon/Moves/`:
- Tackle (Normal, Physical, 40, 100%, 35PP, Priority=0)
- Ember (Fire, Special, 40, 100%, 25PP, Priority=0)
- Water Gun (Water, Special, 40, 100%, 25PP, Priority=0)
- Vine Whip (Grass, Physical, 45, 100%, 25PP, Priority=0)
- Thunder Shock (Electric, Special, 40, 100%, 30PP, Priority=0)
- Quick Attack (Normal, Physical, 40, 100%, 30PP, Priority=1)

Adding a new move = right-click → create UPokemon_Move asset → fill in values. **Zero C++ code.**
