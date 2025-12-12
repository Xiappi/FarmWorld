// Fill out your copyright notice in the Description page of Project Settings.
#include "ThirdPersonCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomGravityMovementComponent.h"

// Sets default values
AThirdPersonCharacter::AThirdPersonCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomGravityMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	//// Disable default gravity
	//GetCharacterMovement()->GravityScale = 0.0f;

	// You can now use your custom movement mode immediately
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);

	// CAMERA SETUP

	// Create Spring Arm (Camera Boom)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	// Distance behind character
	CameraBoom->TargetArmLength = 500.0f;
	// Camera rotates around player
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bInheritPitch = true;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritRoll = false;

	// SMOOTH CAMERA FOLLOW
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 15.f;
	CameraBoom->CameraLagMaxDistance = 50.f;

	// SMOOTH CAMERA ROTATION
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = 10.f;
	CameraBoom->CameraLagMaxTimeStep = 0.02f;

	// CAMERA COLLISION — ENABLED HERE
	// Enable collision checks
	CameraBoom->bDoCollisionTest = true;
	// Collision sphere radius

	CameraBoom->ProbeSize = 15.f;
	// Trace channel (standard camera collision)
	CameraBoom->ProbeChannel = ECC_Camera;
	// Smoother collision behavior
	CameraBoom->bUseCameraLagSubstepping = true;
	// Angle the camera slightly downward
	CameraBoom->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));

	// Prevents camera jitter when very close  
	CameraBoom->bDoCollisionTest = true;

	// Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Camera does not rotate itself
	FollowCamera->bUsePawnControlRotation = false;

	// Adventure-style movement
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

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





