// Fill out your copyright notice in the Description page of Project Settings.
#include "ThirdPersonCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomGravityMovementComponent.h"
#include "Components/CapsuleComponent.h"

AThirdPersonCharacter::AThirdPersonCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomGravityMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    PrimaryActorTick.bCanEverTick = true;

    //
    // === ROTATION SETTINGS (CRITICAL) ===
    //

    // Do NOT let the controller decide rotation
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Movement component must not override any rotation
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    MoveComp->bOrientRotationToMovement = false;
    MoveComp->bUseControllerDesiredRotation = false;
    MoveComp->bIgnoreBaseRotation = true;
    MoveComp->SetMovementMode(MOVE_Flying); // Required for spherical gravity
    MoveComp->RotationRate = FRotator(0.f, 0.f, 0.f);

    //
    // === CHARACTER MESH SHOULD FOLLOW FULL ROTATION ===
    //
    GetMesh()->SetUsingAbsoluteRotation(true);  // allows roll + pitch
    GetMesh()->SetRelativeRotation(FRotator::ZeroRotator);


    //
    // === CAMERA SYSTEM ===
    //

    // Create camera boom
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);

    CameraBoom->TargetArmLength = 500.f;

    // Camera SHOULD rotate with the pawn in all axes
    CameraBoom->bUsePawnControlRotation = false;
    CameraBoom->bInheritPitch = true;
    CameraBoom->bInheritYaw = true;
    CameraBoom->bInheritRoll = true;   // REQUIRED so the camera shows the character rolling

    // Camera smoothing
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 15.f;

    CameraBoom->bEnableCameraRotationLag = true;
    CameraBoom->CameraRotationLagSpeed = 10.f;

    // Collision handling
    CameraBoom->bDoCollisionTest = true;
    CameraBoom->ProbeSize = 15.f;
    CameraBoom->ProbeChannel = ECC_Camera;

    // Create camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false; // camera is independent of controller
}


void AThirdPersonCharacter::Tick(float DeltaTime)
{
}

// Called when the game starts or when spawned
void AThirdPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add the mapping context to the player
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}

}

// Called to bind functionality to input
void AThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EI = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EI->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Move);
		EI->BindAction(LookAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Look);

		EI->BindAction(JumpAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::StartJump);
		EI->BindAction(JumpAction, ETriggerEvent::Completed, this, &AThirdPersonCharacter::StopJump);
	}
}

void AThirdPersonCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Forward/back
		const FRotator ControlRot = Controller->GetControlRotation();
		const FRotator YawRot(0, ControlRot.Yaw, 0);

		const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

		AddMovementInput(Forward, MovementVector.X);
		AddMovementInput(Right, MovementVector.Y);
	}
}

void AThirdPersonCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxis = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxis.X);
	AddControllerPitchInput(LookAxis.Y);
}

void AThirdPersonCharacter::StartJump()
{
	Jump();
}

void AThirdPersonCharacter::StopJump()
{
	StopJumping();
}

// Method 1: Instant rotation to face a direction vector
void AThirdPersonCharacter::RotateToFaceDirection(const FVector& Direction)
{
	if (Direction.IsNearlyZero())
		return;

	// Normalize the direction vector
	FVector NormalizedDirection = Direction.GetSafeNormal();
	
	// Create a rotation from the direction vector
	FRotator NewRotation = NormalizedDirection.Rotation();
	
	// Set the actor's rotation directly
	SetActorRotation(NewRotation);
}

// Method 2: Smooth rotation to face a direction vector
void AThirdPersonCharacter::RotateToFaceDirectionSmooth(const FVector& Direction, float DeltaTime, float RotationSpeed)
{
	if (Direction.IsNearlyZero())
		return;

	// Normalize the direction vector
	FVector NormalizedDirection = Direction.GetSafeNormal();
	
	// Calculate target rotation
	FRotator TargetRotation = NormalizedDirection.Rotation();
	
	// Get current rotation
	FRotator CurrentRotation = GetActorRotation();
	
	// Interpolate smoothly to target rotation
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);
	
	// Apply the new rotation
	SetActorRotation(NewRotation);
}

// Method 3: Rotate to face a target location
void AThirdPersonCharacter::RotateToFaceLocation(const FVector& TargetLocation)
{
	// Calculate direction from character to target
	FVector Direction = TargetLocation - GetActorLocation();
	
	// Use the direction-based method
	RotateToFaceDirection(Direction);
}

// Method 4: Smooth rotation to face a target location
void AThirdPersonCharacter::RotateToFaceLocationSmooth(const FVector& TargetLocation, float DeltaTime, float RotationSpeed)
{
	// Calculate direction from character to target
	FVector Direction = TargetLocation - GetActorLocation();
	
	// Use the smooth direction-based method
	RotateToFaceDirectionSmooth(Direction, DeltaTime, RotationSpeed);
}