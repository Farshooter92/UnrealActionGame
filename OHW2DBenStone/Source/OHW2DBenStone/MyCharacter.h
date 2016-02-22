// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PaperCharacter.h"
#include "Constants.h"
#include "MyCharacter.generated.h"

class UInputComponent;

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
	class UPaperFlipbookComponent* upperBodyFB;

	/** Character's lower body */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Paper2D, meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* lowerBodyFB;

public:

	AMyCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

protected:

	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay();
	
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum, meta = (AllowPrivateAccess = "true"))
	EMovementStates CurMovementState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float StrafingStartAngle = 45.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float StrafingEndAngle = 135.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsMovingForwards = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsStrafingLeft = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsMovingBackwards = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsStrafingRight = false;

	UPROPERTY()
	FVector movementDirectionVec;


public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns UpperBody subobject **/
	FORCEINLINE class UPaperFlipbookComponent* GetUpperBody() const { return upperBodyFB; }
	/** Returns LowerBody subobject **/
	FORCEINLINE class UPaperFlipbookComponent* GetLowerBody() const { return lowerBodyFB; }
	
	
};
