// Copyright Epic Games, Inc. All Rights Reserved.


#include "Public/Player/PokemonPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Public/Character/Pokemon_Trainer.h"

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

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("开始战斗！"));
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
