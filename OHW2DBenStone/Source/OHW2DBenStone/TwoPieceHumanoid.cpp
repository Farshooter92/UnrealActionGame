// Fill out your copyright notice in the Description page of Project Settings.

#include "OHW2DBenStone.h"
#include "TwoPieceHumanoid.h"
#include "PaperFlipbookComponent.h"


// Sets default values
ATwoPieceHumanoid::ATwoPieceHumanoid()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Character rotation only changes in Yaw, to prevent the capsule from changing orientation.
	// Ask the Controller for the full rotation if desired (ie for aiming).
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	// Collision
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collision"));
	CapsuleComponent->InitCapsuleSize(34.0f, 88.0f);
	CapsuleComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

	CapsuleComponent->SetSimulatePhysics(true);
	CapsuleComponent->CanCharacterStepUpOn = ECB_No;
	CapsuleComponent->bShouldUpdatePhysicsVolume = true;
	CapsuleComponent->bCheckAsyncSceneOnMove = false;
	CapsuleComponent->SetCanEverAffectNavigation(false);
	CapsuleComponent->bDynamicObstacle = true;
	RootComponent = CapsuleComponent;

	// Movement
	CharacterMovement = CreateDefaultSubobject<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName);
	CharacterMovement->UpdatedComponent = CapsuleComponent;
	CharacterMovement->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	CharacterMovement->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate


	// Arrow
	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->ArrowColor = FColor::Red;
	ArrowComponent->AttachParent = CapsuleComponent;
	ArrowComponent->bIsScreenSizeScaled = true;

	// Set up the upper body flipbook
	UpperBodyFB = CreateOptionalDefaultSubobject<UPaperFlipbookComponent>(TEXT("Upper Body FB"));
	UpperBodyFB->AlwaysLoadOnClient = true;
	UpperBodyFB->AlwaysLoadOnServer = true;
	UpperBodyFB->bOwnerNoSee = false;
	UpperBodyFB->bAffectDynamicIndirectLighting = true;
	UpperBodyFB->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	UpperBodyFB->AttachParent = CapsuleComponent;
	static FName CollisionProfileName(TEXT("CharacterMesh"));
	UpperBodyFB->SetCollisionProfileName(CollisionProfileName);
	UpperBodyFB->bGenerateOverlapEvents = false;
	
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

	// Legs
	LegsActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("Legs Actor"));
	LegsActor->AttachParent = CapsuleComponent;

	// Initialize vector to face the proper heading
	MovementDirectionVec = LegsActor->GetComponentRotation().Vector();

	CurMovementState = EMovementStates::IDLE;

	CurMovementAction = EMovementActions::MOVING_FORWARDS;



}

void ATwoPieceHumanoid::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CharacterMovement && CapsuleComponent)
	{
		CharacterMovement->UpdateNavAgent(*CapsuleComponent);
	}

	if (Controller == NULL && GetNetMode() != NM_Client)
	{
		if (CharacterMovement && CharacterMovement->bRunPhysicsWithNoController)
		{
			CharacterMovement->SetDefaultMovementMode();
		}
	}

	if (UpperBodyFB)
	{
		// force animation tick after movement component updates
		if (UpperBodyFB->PrimaryComponentTick.bCanEverTick && GetCharacterMovement())
		{
			UpperBodyFB->PrimaryComponentTick.AddPrerequisite(GetCharacterMovement(), GetCharacterMovement()->PrimaryComponentTick);
		}
	}
}


// Called when the game starts or when spawned
void ATwoPieceHumanoid::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* pc = Cast<APlayerController>(GetController());
	if (pc)
	{
		pc->bShowMouseCursor = true;
	}
}

// Called every frame
void ATwoPieceHumanoid::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

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

UPawnMovementComponent* ATwoPieceHumanoid::GetMovementComponent() const
{
	return CharacterMovement;
}