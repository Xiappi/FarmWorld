// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGravityMovementComponent.h"
#include "GravityFieldSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

UCustomGravityMovementComponent::UCustomGravityMovementComponent()
{
	// Disable normal gravity entirely
	GravityScale = 0.0f;
}

void UCustomGravityMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	GravitySubsystem = GetWorld()->GetSubsystem<UGravityFieldSubsystem>();
}

void UCustomGravityMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GravitySubsystem)
		return;

	// Query global gravity from subsystem
	CurrentGravity = GravitySubsystem->GetGravityAtLocation(GetActorLocation());

	ApplyCustomGravity(DeltaTime);
	RotateToGravity(DeltaTime);
}

void UCustomGravityMovementComponent::ApplyCustomGravity(float DeltaTime)
{
	if (CurrentGravity.IsNearlyZero())
	{
		return;
	}

	Velocity += CurrentGravity * DeltaTime;

	PhysFlying(DeltaTime, 1);
}

void UCustomGravityMovementComponent::RotateToGravity(float DeltaTime)
{
	if (CurrentGravity.IsNearlyZero())
	{
		return;
	}

	//ACharacter* CharacterOwner = Cast<ACharacter>(UpdatedComponent->GetOwner());
	//if (!CharacterOwner)
	//{
	//	return;
	//}

	FVector GravityUp = -CurrentGravity.GetSafeNormal();
	FVector Forward = CharacterOwner->GetActorForwardVector();
	FRotator CharacterRotation = CharacterOwner->GetActorRotation();

	FRotator TargetRotation = FRotationMatrix::MakeFromXZ(Forward, GravityUp).Rotator();
	FRotator NewRotation = FMath::RInterpTo(CharacterRotation, TargetRotation, DeltaTime, 5.0f);
	
	CharacterOwner->SetActorRotation(NewRotation);

}

void UCustomGravityMovementComponent::SetGravityMovementMode()
{
	// Flying gives full 3D freedom but still respects physics
	SetMovementMode(MOVE_Flying); 
}
