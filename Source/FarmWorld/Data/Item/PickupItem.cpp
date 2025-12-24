// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupItem.h"
#include "Components\SphereComponent.h"
#include <Characters\Player\FarmWorldCharacter.h>
#include <Kismet\KismetMathLibrary.h>

APickupItem::APickupItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create component hierarchy
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetupAttachment(MeshComponent);

	// Mesh settings
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetEnableGravity(false);
	MeshComponent->SetLinearDamping(2.5f);
	MeshComponent->SetAngularDamping(10.f);  

	// Collision sphere settings
	CollisionSphere->InitSphereRadius(CollisionRadius);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//CollisionSphere->SetCollisionObjectType(ECC_PickupItem); // Custom channel SET IN BLUEPRINT
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionSphere->SetGenerateOverlapEvents(true);
	CollisionSphere->SetNotifyRigidBodyCollision(false);
	CollisionSphere->SetSimulatePhysics(false);
}

// Called when the game starts or when spawned
void APickupItem::BeginPlay()
{
	Super::BeginPlay();
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &APickupItem::OnActorBeginOverlap);
}

void APickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector UpVector = -ItemGravityDirection;

	// Check if we are settled and set state
	if (ItemGravityState != EItemGravityState::Settling && IsSettled(ItemGravityDirection))
	{
		// Transition to placed state
		MeshComponent->SetSimulatePhysics(false);

		SetActorRotation(UpVector.ToOrientationRotator());

		ItemGravityState = EItemGravityState::Placed;
	}

	// Rotate relative to 'up' if placed
	if ((ItemGravityState == EItemGravityState::Placed))
	{
		FVector Forward = GetActorForwardVector();
		Forward = FVector::VectorPlaneProject(Forward, UpVector).GetSafeNormal();

		if (Forward.IsNearlyZero())
		{
			Forward = FVector::CrossProduct(UpVector, FVector::RightVector).GetSafeNormal();
		}

		Forward = Forward.RotateAngleAxis(RotationSpeed * DeltaTime, UpVector);
		FRotator NewRotation = FRotationMatrix::MakeFromXZ(Forward, UpVector).Rotator();

		SetActorRotation(NewRotation);		
	}

	// Debug 
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + ItemGravityDirection * 100.f, FColor::Blue, false, 0.f, 0, 2.f);
	DrawDebugSphere(GetWorld(), CollisionSphere->GetComponentLocation(), CollisionRadius, 12, FColor::Red, false, 0.f);
}

/*
	I just wanted to make a note here that this function is called by the gravity system every tick,
	and items "tick" themselves as well. So this means that items have a double tick.
	This can affect performance if there are many items in the world at once. 
	Should all the "tick" logic be moved here instead?
*/
void APickupItem::ApplyGravity(const FVector& GravityDirection, float GravityStrength, float DeltaTime)
{
	ItemGravityDirection = GravityDirection;

	if (!MeshComponent || !MeshComponent->IsSimulatingPhysics())
	{
		return;
	}

	const FVector UpVector = -GravityDirection;

	// Make sure we are upright relative to gravity
	if (ItemGravityState == EItemGravityState::Falling || ItemGravityState == EItemGravityState::Settling)
	{
		FRotator UprightRotation = UpVector.ToOrientationRotator() * 0.5f;
		FQuat Current = GetActorQuat();
		FQuat Target = FQuat::FindBetweenNormals(
			GetActorUpVector(),
			-GravityDirection
		);

		SetActorRotation((Target * Current).Rotator());
	}

	// Apply gravity if we are falling
	if (ItemGravityState == EItemGravityState::Falling)
	{
		const FVector Force = GravityDirection * GravityStrength * MeshComponent->GetMass();
		MeshComponent->AddForce(Force);
	}
}

FVector APickupItem::GetGravitySampleLocation() const
{
	return MeshComponent ? MeshComponent->GetComponentLocation() : GetActorLocation();
}

void APickupItem::OnActorBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Collides with player - try to add to inventory
	AFarmWorldCharacter* Character = Cast<AFarmWorldCharacter>(OtherActor);
	if (Character)
	{

		if (Character->Inventory->AddItem(Item, Quantity))
		{
			Destroy();
		}
	}

	// Collides with anything else - consider this "settled"
	if (ItemGravityState == EItemGravityState::Falling)
	{
		ItemGravityState = EItemGravityState::Settling;
	}
}

bool APickupItem::HasGroundBelow(const FVector& GravityDir, float Distance) const
{
	FVector Start = GetActorLocation();
	FVector End = Start + GravityDir * Distance;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	return GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_WorldStatic,
		Params
	);
}

bool APickupItem::IsFallingSlowly(const FVector& GravityDir, float Threshold) const
{
	if (!MeshComponent)
	{
		return false;
	}

	FVector Velocity = MeshComponent->GetPhysicsLinearVelocity();
	float VerticalSpeed = FVector::DotProduct(Velocity, GravityDir);

	return FMath::Abs(VerticalSpeed) < Threshold;
}

bool APickupItem::IsRotationallyStable(float Threshold) const
{
	if (!MeshComponent)
	{
		return false;
	}

	FVector AngularVel = MeshComponent->GetPhysicsAngularVelocityInDegrees();

	return AngularVel.Size() < Threshold;
}

bool APickupItem::IsSettled(const FVector& GravityDir) const
{
	return 
		HasGroundBelow(GravityDir, 20.f) && 
		IsFallingSlowly(GravityDir, 8.f) && 
		IsRotationallyStable(10.f);

}

