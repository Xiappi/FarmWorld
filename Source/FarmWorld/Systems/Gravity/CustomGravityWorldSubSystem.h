// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GravityAttractorComponent.h"
#include "CustomGravityAsyncCallback.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <GravityConsumer.h>
#include "CustomGravityWorldSubSystem.generated.h"

UCLASS()
class FARMWORLD_API UCustomGravityWorldSubSystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public: // UTickableWorldSubsystem overrides
	virtual void Deinitialize() override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual TStatId GetStatId() const override;
	void UpdateCMCGravities();
	void HandleGravityConsumers(float DeltaTime);
	virtual void Tick(float DeltaTime) override;

	// Keep track of any attractors (optional)
	void AddAttractor(UGravityAttractorComponent* GravityAttractorComponent);
	void RemoveAttractor(UGravityAttractorComponent* GravityAttractorComponent);
	FVector ComputeGravityAtLocation(const FVector& SampleLocation);
protected:
	TArray<TWeakObjectPtr<UGravityAttractorComponent>> Attractors;

	// Track the CMC components
	void AddActorToTrackedCharacters(AActor* Actor);
	void RemoveActorFromTrackedCharacters(AActor* Actor);

	FDelegateHandle ActorSpawnedHandle;
	FDelegateHandle ActorDestroyedHandle;

	TArray<TWeakObjectPtr<UCharacterMovementComponent>> TrackedCharacterMovementComponents;
	TArray<TWeakObjectPtr<UObject>> GravityConsumers;
public:
	// Register async callback with physics system.
	virtual void RegisterAsyncCallback();
	virtual bool IsAsyncCallbackRegistered() const;
	void AddGravityAttractorData(const FGravityAttractorData& InputData) const;
	FCustomGravityAsyncCallback* AsyncCallback = nullptr;
};