// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GravityConsumer.generated.h"

UINTERFACE(BlueprintType)
class UGravityConsumer : public UInterface
{
	GENERATED_BODY()
};

class IGravityConsumer
{
	GENERATED_BODY()

public:

	/** Called every tick by gravity system */
	virtual void ApplyGravity(const FVector& GravityDirection, float GravityStrength, float DeltaTime) = 0;

	/** Where gravity should be evaluated from (pivot choice) */
	virtual FVector GetGravitySampleLocation() const = 0;

	/** Should gravity currently affect this actor? */
	virtual bool IsGravityEnabled() const { return true; };

	/** Optional: notify when dominant attractor changes */
	virtual void OnGravitySourceChanged(UObject* NewSource) {};
};


