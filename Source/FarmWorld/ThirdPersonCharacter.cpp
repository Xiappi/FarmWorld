// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h" // Add this include at the top of your file

// Sets default values
AThirdPersonCharacter::AThirdPersonCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create Spring Arm
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraBoom->TargetArmLength = 400.0f;       // Distance behind character
	CameraBoom->bUsePawnControlRotation = true; // Rotate arm based on controller
	CameraBoom->bInheritPitch = true;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritRoll = false;

	// Create Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera follows boom, not pawn

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

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





