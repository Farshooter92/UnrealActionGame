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

	// Character rotation only changes in Yaw, to prevent the capsule from changing orientation.
	// Ask the Controller for the full rotation if desired (ie for aiming).
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	// Collision
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	GetCapsuleComponent()->SetSimulatePhysics(true);
	GetCapsuleComponent()->CanCharacterStepUpOn = ECB_No;
	RootComponent = GetCapsuleComponent();

	// Movement
	GetCharacterMovement()->UpdatedComponent = CapsuleComponent;
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate

	// Create a camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(GetCapsuleComponent());
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

	// Set up the upper body
	UpperBodyFB = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("UpperBody"));
	UpperBodyFB->AttachTo(GetCapsuleComponent());
	UpperBodyFB->AlwaysLoadOnClient = true;
	UpperBodyFB->AlwaysLoadOnServer = true;
	UpperBodyFB->bOwnerNoSee = false;
	UpperBodyFB->bAffectDynamicIndirectLighting = true;
	UpperBodyFB->bGenerateOverlapEvents = false;
	UpperBodyFB->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Set up the lower body
	LowerBodyFB = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("LowerBody"));
	LowerBodyFB->AttachTo(GetCapsuleComponent());
	UpperBodyFB->AlwaysLoadOnClient = true;
	UpperBodyFB->AlwaysLoadOnServer = true;
	UpperBodyFB->bOwnerNoSee = false;
	UpperBodyFB->bAffectDynamicIndirectLighting = true;
	UpperBodyFB->bGenerateOverlapEvents = false;
	UpperBodyFB->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Initialize vector to face the proper heading
	MovementDirectionVec = LowerBodyFB->GetComponentRotation().Vector();

	CurMovementState = EMovementStates::IDLE;

	CurMovementAction = EMovementActions::MOVING_FORWARDS;
}

void AMyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (UpperBodyFB)
	{
		// force animation tick after movement component updates
		if (UpperBodyFB->PrimaryComponentTick.bCanEverTick && GetCharacterMovement())
		{
			UpperBodyFB->PrimaryComponentTick.AddPrerequisite(GetCharacterMovement(), GetCharacterMovement()->PrimaryComponentTick);
		}
	}

	if (LowerBodyFB)
	{
		// force animation tick after movement component updates
		if (LowerBodyFB->PrimaryComponentTick.bCanEverTick && GetCharacterMovement())
		{
			LowerBodyFB->PrimaryComponentTick.AddPrerequisite(GetCharacterMovement(), GetCharacterMovement()->PrimaryComponentTick);
		}
	}
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
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

	UpperBodyFB->SetWorldRotation(UpperBodyRot);

	// Determine Heading of lower Body
	FVector upperBodyVec = UpperBodyRot.Vector();
	upperBodyVec.Normalize();

	FVector vel = GetVelocity();
	vel.Z = 0.0f; // Ignore the movement in the vertical direction like when he is falling.

	// Calculate Movement Vector
	if (!vel.Equals(FVector::ZeroVector))
	{
		MovementDirectionVec = vel;
	}

	MovementDirectionVec.Normalize();

	// Calculate Angle
	float dotProduct = FVector::DotProduct(upperBodyVec, MovementDirectionVec);
	float magnitudes = upperBodyVec.Size() * MovementDirectionVec.Size();

	float AimAtAngle = FMath::RadiansToDegrees(acosf(dotProduct / magnitudes));

	FVector sideVector = UpperBodyRot.Vector().RotateAngleAxis(90.0f, FVector(0, 0, 1));

	// Greater than 0 is left, Less than zero is right
	float sideDirection = FVector::DotProduct(sideVector, MovementDirectionVec);

	// He is looking left
	if (sideDirection < 0 && AimAtAngle > StrafingStartAngle && AimAtAngle < StrafingEndAngle)
	{
		CurMovementAction = EMovementActions::STRAFING_LEFT;
	}

	// He is looking right
	if (sideDirection > 0 && AimAtAngle > StrafingStartAngle && AimAtAngle < StrafingEndAngle)
	{
		CurMovementAction = EMovementActions::STRAFING_RIGHT;
	}

	// Is he moving forward in relation to where he is facing
	if (AimAtAngle < StrafingStartAngle)
	{
		CurMovementAction = EMovementActions::MOVING_FORWARDS;
	}

	// Is he moving backward in relation to where he is facing
	if (AimAtAngle > StrafingEndAngle)
	{
		CurMovementAction = EMovementActions::MOVING_BACKWARDS;
	}

	// Debug
	// Draw Vectors for debugging
	// UE_LOG(LogTemp, Warning, TEXT("Angle %f"), AimAtAngle);

	DrawDebugLine(GetWorld(), UpperBodyFB->GetComponentLocation(), UpperBodyFB->GetComponentLocation() + (upperBodyVec * 500), FColor::Red, false, -1, 0, 12.333);
	DrawDebugLine(GetWorld(), UpperBodyFB->GetComponentLocation(), UpperBodyFB->GetComponentLocation() + (sideVector * 500), FColor::Green, false, -1, 0, 12.333);
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + (MovementDirectionVec * 500), FColor::Yellow, false, -1, 0, 12.333);

	// Movement
	// UE_LOG(LogTemp, Warning, TEXT("Moving Forward %d, Strafing Left %d, Moving Backward %d, Strafing Right %d"), bIsMovingForwards, bIsStrafingLeft, bIsMovingBackwards, bIsStrafingRight);
	// UE_LOG(LogTemp, Warning, TEXT("Dot Product: %f"), dotProduct);
	// UE_LOG(LogTemp, Warning, TEXT("Side Direction: %f"), sideDirection);
	UE_LOG(LogTemp, Warning, TEXT("MovementDirectionVec: X = %f, Y = %f, Z = %f"), MovementDirectionVec.X, MovementDirectionVec.Y, MovementDirectionVec.Z);

	switch (CurMovementAction)
	{
	case EMovementActions::STRAFING_LEFT:
		UE_LOG(LogTemp, Warning, TEXT("STRAFING_LEFT"));
		break;
	case EMovementActions::STRAFING_RIGHT:
		UE_LOG(LogTemp, Warning, TEXT("STRAFING_RIGHT"));
		break;
	case EMovementActions::MOVING_FORWARDS:
		UE_LOG(LogTemp, Warning, TEXT("MOVING_FORWARDS"));
		break;
	case EMovementActions::MOVING_BACKWARDS:
		UE_LOG(LogTemp, Warning, TEXT("MOVING_BACKWARDS"));
		break;
	}
}




