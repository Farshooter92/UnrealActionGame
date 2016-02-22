// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "Constants.h"
#include "TwoPieceHumanoid.generated.h"

class UInputComponent;

UCLASS()
class OHW2DBENSTONE_API ATwoPieceHumanoid : public APawn
{
	GENERATED_BODY()

	UPROPERTY()
	class UArrowComponent* ArrowComponent;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Movement component used for movement logic in various movement modes (walking, falling, etc), containing relevant settings and functions to control movement. */
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCharacterMovementComponent* CharacterMovement;

	/** The CapsuleComponent being used for movement collision (by CharacterMovement). Always treated as being vertically aligned in simple collision check functions. */
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CapsuleComponent;

	/** Flipbook for the upperbody */
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* UpperBodyFB;

	// The actor used as the legs.
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* LegsActor;

public:
	// Sets default values for this pawn's properties
	ATwoPieceHumanoid();

protected:

	// Allow actors to initialize themselves on the C++ side.
	virtual void PostInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual UPawnMovementComponent* GetMovementComponent() const override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum, meta = (AllowPrivateAccess = "true"))
	EMovementStates CurMovementState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	EMovementActions CurMovementAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float StrafingStartAngle = 45.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float StrafingEndAngle = 135.0f;

	UPROPERTY()
	FVector MovementDirectionVec;

public:
	//////////////////////////////////////////////////////////////////////////
	// Character inlines

	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns ArrowComponent subobject **/
	FORCEINLINE UArrowComponent* GetArrowComponent() const { return ArrowComponent; }
	/** Returns CharacterMovement subobject **/
	FORCEINLINE UCharacterMovementComponent* GetCharacterMovement() const { return CharacterMovement; }
	/** Returns CapsuleComponent subobject **/
	FORCEINLINE UCapsuleComponent* GetCapsuleComponent() const { return CapsuleComponent; }
	/** Returns UpperBodyFB subobject **/
	FORCEINLINE UPaperFlipbookComponent* GetUpperBodyFB() const { return UpperBodyFB; }
	/** Returns LegsActor subobject **/
	FORCEINLINE UChildActorComponent* GetUpperLegsActor() const { return LegsActor; }
};
