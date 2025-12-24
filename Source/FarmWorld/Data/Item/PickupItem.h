// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemDefinition.h"
#include <Components/StaticMeshComponent.h>
#include <Components/SphereComponent.h>
#include "GravityConsumer.h"
#include "PickupItem.generated.h"

UENUM(BlueprintType)
enum class EItemGravityState : uint8
{
	Falling		UMETA(DisplayName = "Falling"),    // physics ON
	Settling	UMETA(DisplayName = "Settling"),   // physics ON, checking ground
	Placed		UMETA(DisplayName = "Placed")      // physics OFF, kinematic
};

UCLASS()
class FARMWORLD_API APickupItem : public AActor, public IGravityConsumer
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupItem();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionSphere;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void Tick(float DeltaTime) override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	UItemDefinition* Item;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	int32 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	float CollisionRadius = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	float RotationSpeed = 50.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	EItemGravityState ItemGravityState = EItemGravityState::Falling;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	FVector ItemGravityDirection;

	// Inherited via IGravityConsumer
	void ApplyGravity(const FVector& GravityDirection, float GravityStrength, float DeltaTime) override;
	FVector GetGravitySampleLocation() const override;

protected:
	UFUNCTION()
	void OnActorBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	bool HasGroundBelow(const FVector& GravityDir, float Distance) const;
	bool IsFallingSlowly(const FVector& GravityDir, float Threshold) const;
	bool IsRotationallyStable(float Threshold) const;
	bool IsSettled(const FVector& GravityDir) const;
};