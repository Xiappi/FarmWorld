// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "ThirdPersonCharacter.generated.h"

UCLASS()
class FARMWORLD_API AThirdPersonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AThirdPersonCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;


protected:
	virtual void BeginPlay() override;
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartJump();
	void StopJump();

	// Rotation methods - examples for rotating to face a vector
	// Method 1: Instant rotation to face a direction vector
	void RotateToFaceDirection(const FVector& Direction);
	
	// Method 2: Smooth rotation to face a direction vector
	void RotateToFaceDirectionSmooth(const FVector& Direction, float DeltaTime, float RotationSpeed = 5.0f);
	
	// Method 3: Rotate to face a target location
	void RotateToFaceLocation(const FVector& TargetLocation);
	
	// Method 4: Smooth rotation to face a target location
	void RotateToFaceLocationSmooth(const FVector& TargetLocation, float DeltaTime, float RotationSpeed = 5.0f);

public:	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;

};
