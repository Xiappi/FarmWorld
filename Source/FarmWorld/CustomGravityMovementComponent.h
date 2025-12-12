// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GravityFieldSubsystem.h"
#include "CustomGravityMovementComponent.generated.h"

UCLASS()
class FARMWORLD_API UCustomGravityMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UCustomGravityMovementComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	// Enable gravity movement mode
	void SetGravityMovementMode();

protected:
	// Where gravity comes from
	UGravityFieldSubsystem* GravitySubsystem;

	// Cached gravity vector from all planets
	FVector CurrentGravity;

	// Handles applying gravity + velocity
	void ApplyCustomGravity(float DeltaTime);

	// Align character capsule to face “away” from planet
	void RotateToGravity(float DeltaTime);

};
