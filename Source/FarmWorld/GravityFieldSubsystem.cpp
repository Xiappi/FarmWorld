// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityFieldSubsystem.h"

void UGravityFieldSubsystem::RegisterPlanet(APlanetGravity* Planet)
{
	Planets.Add(Planet);
}

FVector UGravityFieldSubsystem::GetGravityAtLocation(const FVector& Location) const
{
	FVector Gravity = FVector::ZeroVector;
	for (APlanetGravity* Planet : Planets)
	{
		if (!Planet)
		{
			UE_LOG(LogTemp, Warning, TEXT("Non-planet registered in gravity subsystem"));
			continue;
		}

		Gravity += Planet->GetGravityVectorAtPoint(Location);
	}
	
	return Gravity;
}
