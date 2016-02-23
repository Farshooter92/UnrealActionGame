// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PaperCharacter.h"
#include "Constants.h"
#include "MyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class OHW2DBENSTONE_API AMyCharacter : public APaperCharacter
{
	GENERATED_BODY()

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Character's upper body */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Paper2D, meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* UpperBodyFB;

	/** Character's lower body */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Paper2D, meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* LowerBodyFB;

public:

	AMyCharacter();

protected:

	// Allow actors to initialize themselves on the C++ side.
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enum, meta = (AllowPrivateAccess = "true"))
	EMovementStates CurMovementState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	EMovementActions CurMovementAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float StrafingStartAngle = 45.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float StrafingEndAngle = 135.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enum, meta = (AllowPrivateAccess = "true"))
	FVector MovementDirectionVec;


public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns UpperBody subobject **/
	FORCEINLINE class UPaperFlipbookComponent* GetUpperBody() const { return UpperBodyFB; }
	/** Returns LowerBody subobject **/
	FORCEINLINE class UPaperFlipbookComponent* GetLowerBody() const { return LowerBodyFB; }
	
	
};
