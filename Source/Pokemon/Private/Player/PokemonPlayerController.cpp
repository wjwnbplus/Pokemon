// Copyright Epic Games, Inc. All Rights Reserved.


#include "Public/Player/PokemonPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Public/Character/Pokemon_Trainer.h"
#include "Battle/BattleManager.h"
#include "Character/Pokemon_BattleNPC.h"

void APokemonPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
}

void APokemonPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
			
		}

		// Bind input actions
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			// Jumping
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APokemonPlayerController::OnJumpStarted);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APokemonPlayerController::OnJumpEnded);

			// Moving
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APokemonPlayerController::OnMove);
			
			// Looking
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APokemonPlayerController::OnLook);
			EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &APokemonPlayerController::OnLook);

			// Interact
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APokemonPlayerController::OnInteract);
		}
	}
}

void APokemonPlayerController::OnJumpStarted()
{
	if (APokemon_Trainer* PokemonChar = Cast<APokemon_Trainer>(GetPawn()))
	{
		PokemonChar->DoJumpStart();
	}
}

void APokemonPlayerController::OnJumpEnded()
{
	if (APokemon_Trainer* PokemonChar = Cast<APokemon_Trainer>(GetPawn()))
	{
		PokemonChar->DoJumpEnd();
	}
}

void APokemonPlayerController::OnInteract()
{
	APokemon_Trainer* Trainer = Cast<APokemon_Trainer>(GetPawn());
	if (!Trainer) return;

	APokemon_BattleNPC* NPC = Trainer->GetOverlappingNPC();
	if (!NPC) return;

	// 检查双方是否有宝可梦
	if (NPC->GetPokemonParty().Num() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("This trainer has no Pokemon!"));
		return;
	}

	if (Trainer->GetPokemonParty().Num() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("You have no Pokemon!"));
		return;
	}

	// 生成对战管理器
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ABattleManager* BattleManager = GetWorld()->SpawnActor<ABattleManager>(ABattleManager::StaticClass(), SpawnParams);
	if (BattleManager)
	{
		BattleManager->StartBattle(Trainer, NPC);
	}
}

void APokemonPlayerController::OnMove(const FInputActionValue& Value)
{
	if (APokemon_Trainer* PokemonChar = Cast<APokemon_Trainer>(GetPawn()))
	{
		FVector2D MovementVector = Value.Get<FVector2D>();
		PokemonChar->DoMove(MovementVector.X, MovementVector.Y);
	}
}

void APokemonPlayerController::OnLook(const FInputActionValue& Value)
{
	if (APokemon_Trainer* PokemonChar = Cast<APokemon_Trainer>(GetPawn()))
	{
		FVector2D LookAxisVector = Value.Get<FVector2D>();
		PokemonChar->DoLook(LookAxisVector.X, LookAxisVector.Y);
	}
}
