// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FGravityAttractorData
{
	FVector Location;
	double MassDotG;
};

struct FARMWORLD_API FCustomGravityAsyncInput : public Chaos::FSimCallbackInput
{
	TArray<FGravityAttractorData> GravityAttractorsData;

	void Reset()
	{
		GravityAttractorsData.Empty();
	}
};


class FARMWORLD_API FCustomGravityAsyncCallback : public Chaos::TSimCallbackObject<
	FCustomGravityAsyncInput, // A custom Input class to convey data from the DT to the PT
	Chaos::FSimCallbackNoOutput, // No need to output anything here
	Chaos::ESimCallbackOptions::PreIntegrate | Chaos::ESimCallbackOptions::Presimulate> // The hooks we want to register into -> Implement the corresponding virtual methods below
{
public:
	FCustomGravityAsyncCallback();
	virtual ~FCustomGravityAsyncCallback() override;

	virtual void OnPreSimulate_Internal() override;
	virtual void OnPreIntegrate_Internal() override;

protected:
	static double GravitationalConstant;
};
