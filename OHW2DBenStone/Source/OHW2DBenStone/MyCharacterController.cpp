// Fill out your copyright notice in the Description page of Project Settings.

#include "OHW2DBenStone.h"
#include "MyCharacterController.h"

AMyCharacterController::AMyCharacterController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AMyCharacterController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAxis("MoveForward", this, &AMyCharacterController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMyCharacterController::MoveRight);
}

void AMyCharacterController::MoveForward(float Value)
{
	if ((this != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = this->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		GetPawn()->AddMovementInput(Direction, Value);
	}
}

void AMyCharacterController::MoveRight(float Value)
{
	if ((this != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = this->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		GetPawn()->AddMovementInput(Direction, Value);
	}
}


