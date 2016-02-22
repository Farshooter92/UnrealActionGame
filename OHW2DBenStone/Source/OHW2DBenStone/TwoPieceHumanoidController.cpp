// Fill out your copyright notice in the Description page of Project Settings.

#include "OHW2DBenStone.h"
#include "TwoPieceHumanoidController.h"

ATwoPieceHumanoidController::ATwoPieceHumanoidController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

// Called to bind functionality to input
void ATwoPieceHumanoidController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAxis("MoveForward", this, &ATwoPieceHumanoidController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ATwoPieceHumanoidController::MoveRight);

}

void ATwoPieceHumanoidController::MoveForward(float Value)
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

void ATwoPieceHumanoidController::MoveRight(float Value)
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


