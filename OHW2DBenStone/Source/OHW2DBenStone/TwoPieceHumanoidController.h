// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "TwoPieceHumanoidController.generated.h"

class UInputComponent;

/**
 * 
 */
UCLASS()
class OHW2DBENSTONE_API ATwoPieceHumanoidController : public APlayerController
{
	GENERATED_BODY()
public:
	ATwoPieceHumanoidController();

protected:

	// Called to bind functionality to input
	virtual void SetupInputComponent() override;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);
	
	
};
