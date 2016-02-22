// Fill out your copyright notice in the Description page of Project Settings.

#include "OHW2DBenStone.h"
#include "MyCharacter.h"
#include "PaperFlipbookComponent.h"

//////////////////////////////////////////////////////////////////////////
// AThirdPersonCPPCharacter

AMyCharacter::AMyCharacter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetConstraintMode(EDOFMode::XYPlane);

	// set our turn rates for input
	BaseTurnRate = 45.f;

	// Set up the upper body
	upperBodyFB = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("UpperBody"));
	upperBodyFB->AttachTo(RootComponent);

	// Set up the lower body
	lowerBodyFB = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("LowerBody"));
	lowerBodyFB->AttachTo(RootComponent);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Initialize vector to face the proper heading
	movementDirectionVec = lowerBodyFB->GetComponentRotation().Vector();

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.0f; // The camera follows at this distance behind the character	
	CameraBoom->RelativeRotation = FRotator(-90.f, 0.f, 0.f);
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->ProjectionMode = ECameraProjectionMode::Orthographic;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Core Game Loop

void AMyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Get mouse position
	APlayerController* pc = Cast<APlayerController>(GetController());

	float mouseX = 0.f;
	float mouseY = 0.f;

	FVector mousePos = FVector::ZeroVector;

	if (pc->GetMousePosition(mouseX, mouseY))
	{
		mousePos = FVector(mouseX, mouseY, 0.0f);
		// UE_LOG(LogTemp, Warning, TEXT("Mouse X %f: MouseY %f"), mouseX, mouseY);
	}

	// Calculate look at direction
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());

	FVector middleOfScreen = FVector(ViewportSize.X / 2, ViewportSize.Y / 2, 0.0f);

	FVector directionToMouse = mousePos - middleOfScreen;

	FRotator UpperBodyRot = directionToMouse.Rotation();

	UpperBodyRot.Roll = -90.0f;
	UpperBodyRot.Pitch = 0.0f;
	UpperBodyRot.Yaw = UpperBodyRot.Yaw + 90.0f;

	upperBodyFB->SetWorldRotation(UpperBodyRot);
	
	// Determine Heading of lower Body
	FVector upperBodyVec = UpperBodyRot.Vector();
	upperBodyVec.Normalize();

	FVector vel = GetVelocity();
	vel.Z = 0.0f; // Ignore the movement in the vertical direction like when he is falling.

	// Calculate Movement Vector
	if (!vel.Equals(FVector::ZeroVector))
	{
		movementDirectionVec = vel;
	}

	movementDirectionVec.Normalize();

	// Calculate Angle
	float dotProduct = FVector::DotProduct(upperBodyVec, movementDirectionVec);
	float magnitudes = upperBodyVec.Size() * movementDirectionVec.Size();

	float AimAtAngle = FMath::RadiansToDegrees(acosf(dotProduct/magnitudes));

	FVector sideVector = UpperBodyRot.Vector().RotateAngleAxis(90.0f, FVector(0, 0, 1));

	// Greater than 0 is left, Less than zero is right
	float sideDirection = FVector::DotProduct(sideVector, movementDirectionVec);

	// He is looking left
	if (sideDirection < 0 && AimAtAngle > StrafingStartAngle && AimAtAngle < StrafingEndAngle)
	{
		bIsStrafingLeft = true;
	}
	else
	{
		bIsStrafingLeft = false;
	}

	// He is looking right
	if (sideDirection > 0 && AimAtAngle > StrafingStartAngle && AimAtAngle < StrafingEndAngle)
	{
		bIsStrafingRight = true;
	}
	else
	{
		bIsStrafingRight = false;
	}

	// Is he moving forward in relation to where he is facing
	if (AimAtAngle < StrafingStartAngle)
	{
		bIsMovingForwards = true;
	}
	else
	{
		bIsMovingForwards = false;
	}

	// Is he moving backward in relation to where he is facing
	if (AimAtAngle > StrafingEndAngle)
	{
		bIsMovingBackwards = true;
	}
	else
	{
		bIsMovingBackwards = false;
	}

	// Debug
	// Draw Vectors for debugging
	// UE_LOG(LogTemp, Warning, TEXT("Angle %f"), AimAtAngle);

	DrawDebugLine(GetWorld(), upperBodyFB->GetComponentLocation(), upperBodyFB->GetComponentLocation() + (upperBodyVec * 500), FColor::Red, false, -1, 0, 12.333);
	DrawDebugLine(GetWorld(), upperBodyFB->GetComponentLocation(), upperBodyFB->GetComponentLocation() + (sideVector * 500), FColor::Green, false, -1, 0, 12.333);
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + (movementDirectionVec * 500), FColor::Yellow, false, -1, 0, 12.333);

	// Movement
	UE_LOG(LogTemp, Warning, TEXT("Moving Forward %d, Strafing Left %d, Moving Backward %d, Strafing Right %d"), bIsMovingForwards, bIsStrafingLeft, bIsMovingBackwards, bIsStrafingRight);	
	// UE_LOG(LogTemp, Warning, TEXT("Dot Product: %f"), dotProduct);
	// UE_LOG(LogTemp, Warning, TEXT("Side Direction: %f"), sideDirection);
	UE_LOG(LogTemp, Warning, TEXT("MovementDirectionVec: X = %f, Y = %f, Z = %f"), movementDirectionVec.X, movementDirectionVec.Y, movementDirectionVec.Z);

}

//////////////////////////////////////////////////////////////////////////
// Input

void AMyCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AMyCharacter::TurnAtRate);
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* pc = Cast<APlayerController>(GetController());
	if (pc)
	{
		pc->bShowMouseCursor = true;
	}
}

void AMyCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMyCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMyCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}




