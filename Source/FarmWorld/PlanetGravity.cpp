// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetGravity.h"
#include "GravityFieldSubsystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlanetGravity::APlanetGravity()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlanetGravity::BeginPlay()
{
	Super::BeginPlay();

	PlanetCenter = GetActorLocation();
	
	if (UGravityFieldSubsystem* Subsystem = GetWorld()->GetSubsystem<UGravityFieldSubsystem>())
	{
		Subsystem->RegisterPlanet(this);
	}
}

// Called every frame
void APlanetGravity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebugSphere(GetWorld(), PlanetCenter, GravityRadius, 32, FColor::Blue, false, -1.0f, 0, 2.0f);
}

FVector APlanetGravity::GetGravityVectorAtPoint(const FVector& Location) const
{
	FVector Direction = GetGravityDirection(Location);
	float Distance = GetDistanceToCenter(Location);

	if(Distance > GravityRadius)
	{
		return FVector::ZeroVector;
	}

	float Falloff = 1.0f - (Distance / GravityRadius);
	float Strength = GravityStrength * Falloff;

	return Direction * Strength;
}

FVector APlanetGravity::GetGravityDirection(const FVector& Location) const
{
	return (PlanetCenter - Location).GetSafeNormal();
}

float APlanetGravity::GetDistanceToCenter(const FVector& Location) const
{
	return FVector::Distance(PlanetCenter, Location);
}

