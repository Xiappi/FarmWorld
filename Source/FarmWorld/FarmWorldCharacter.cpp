// Copyright Epic Games, Inc. All Rights Reserved.

#include "FarmWorldCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "FarmWorld.h"
#include "GravityController.h"

AFarmWorldCharacter::AFarmWorldCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AFarmWorldCharacter::BeginPlay()
{
	Super::BeginPlay();

	UCharacterMovementComponent* CMC = GetCharacterMovement();

	CMC->BrakingDecelerationFalling = 0.f;
	CMC->BrakingFriction = 0.f;
	CMC->FallingLateralFriction = 0.f;
	CMC->bUseSeparateBrakingFriction = true;
}

void AFarmWorldCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AFarmWorldCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFarmWorldCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AFarmWorldCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFarmWorldCharacter::Look);
	}
	else
	{
		UE_LOG(LogFarmWorld, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AFarmWorldCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UCharacterMovementComponent* CMC = GetCharacterMovement();

	if (!CMC)
	{
		UE_LOG(LogFarmWorld, Error, TEXT("'%s' Failed to get Character Movement Component!"), *GetNameSafe(this));
		return;
	}

	FVector GravityDirection = CMC->GetGravityDirection();
	FVector UpDir = -GravityDirection;
	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = TraceStart + GravityDirection * TraceOffset;

	FRotator GravityRot = FRotationMatrix::MakeFromZ(UpDir).Rotator();
	FQuat GravityQuat = GravityRot.Quaternion();

	FCollisionQueryParams Params = FCollisionQueryParams(SCENE_QUERY_STAT(PlanetGroundProbe), /*bTraceComplex*/ false);
	Params.AddIgnoredActor(this);  // if inside Character class; otherwise GetOwner()


	FHitResult Hit;
	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		TraceStart,
		TraceEnd,
		GravityQuat,
		ECC_Visibility,
		FCollisionShape::MakeSphere(GroundProbeRadius),
		Params
	);
	DrawDebugLine(
		GetWorld(),
		TraceStart,
		TraceEnd,
		bHit ? FColor::Green : FColor::Red,
		false,
		-1.f,
		0,
		1.f
	);

	DrawDebugSphere(
		GetWorld(),
		TraceEnd,
		GroundProbeRadius,
		12,
		bHit ? FColor::Green : FColor::Red,
		false,
		-1.f,
		0,
		1.f
	);

	BGrounded = CMC->IsMovingOnGround() || CMC->CurrentFloor.IsWalkableFloor() || bHit;

	if (BGrounded)
	{
		CMC->SetMovementMode(MOVE_Walking);
	}
}

void AFarmWorldCharacter::Move(const FInputActionValue& Value)
{

	if (!BGrounded)
	{
		return;
	}

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	FRotator rotation = GetControlRotation();
	FVector gravityDirection = GetCharacterMovement()->GetGravityDirection();
	FRotator relativeGravityRotation = AGravityController::GetGravityRelativeRotation(rotation, gravityDirection);

	// Handle right/left movement
	FRotator yawRollRotator = FRotator(0, relativeGravityRotation.Yaw, relativeGravityRotation.Roll);
	FRotator worldRotationRight = AGravityController::GetGravityWorldRotation(yawRollRotator, gravityDirection);
	FVector rightVector = FRotationMatrix(worldRotationRight).GetUnitAxis(EAxis::Y);
	AddMovementInput(rightVector, MovementVector.X);

	// Handle forward/backward movement
	FRotator yawRotator = FRotator(0, relativeGravityRotation.Yaw, 0);
	FRotator worldRotationForward = AGravityController::GetGravityWorldRotation(yawRotator, gravityDirection);
	FVector forwardVector = FRotationMatrix(worldRotationForward).GetUnitAxis(EAxis::X);
	AddMovementInput(forwardVector, MovementVector.Y);

}

void AFarmWorldCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AFarmWorldCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AFarmWorldCharacter::DoJumpStart()
{
	Super::Jump();
}

void AFarmWorldCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}
