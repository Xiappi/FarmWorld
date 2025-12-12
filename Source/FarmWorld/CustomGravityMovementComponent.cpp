// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGravityMovementComponent.h"
#include "GravityFieldSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

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
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character) return;

    USkeletalMeshComponent* Mesh = Character->GetMesh();
    if (!Mesh) return;

    const FVector GravityUp = (-CurrentGravity).GetSafeNormal();

    FVector Forward = Mesh->GetForwardVector();
    Forward = FVector::VectorPlaneProject(Forward, GravityUp).GetSafeNormal();
    const FRotator TargetRotation = FRotationMatrix::MakeFromXZ(Forward, GravityUp).Rotator();

    const FRotator NewRotation = FMath::RInterpTo(
        Mesh->GetComponentRotation(),
        TargetRotation,
        DeltaTime,
        5.f
    );

    Mesh->SetWorldRotation(NewRotation, false, nullptr, ETeleportType::TeleportPhysics);
}

void UCustomGravityMovementComponent::SetGravityMovementMode()
{
	// Flying gives full 3D freedom but still respects physics
	SetMovementMode(MOVE_Flying);
}
