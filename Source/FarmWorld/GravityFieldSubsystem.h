// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PlanetGravity.h"
#include "GravityFieldSubsystem.generated.h"

UCLASS()
class FARMWORLD_API UGravityFieldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	void RegisterPlanet(APlanetGravity* Planet);
	FVector GetGravityAtLocation(const FVector& Location) const;

private:
	UPROPERTY(VisibleAnywhere)
	TArray<APlanetGravity*> Planets;
};
