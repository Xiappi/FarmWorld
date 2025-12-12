// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlanetGravity.generated.h"

UCLASS()
class FARMWORLD_API APlanetGravity : public AActor
{
	GENERATED_BODY()
	
public:	
	APlanetGravity();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// Strength of the gravity force
	UPROPERTY(EditAnywhere, Category="Gravity")
	float GravityStrength = 2000.0f;

	// How far the gravity field reaches
	UPROPERTY(EditAnywhere, Category = "Gravity")
	float GravityRadius = 3000.f;

	FVector GetGravityVectorAtPoint(const FVector& Location) const;
	FVector GetGravityDirection(const FVector& Location) const;
	float GetDistanceToCenter(const FVector& Location) const;


private:
	FVector PlanetCenter = FVector::ZeroVector;

};
