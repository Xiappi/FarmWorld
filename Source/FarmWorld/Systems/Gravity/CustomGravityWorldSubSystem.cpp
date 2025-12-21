// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGravityWorldSubSystem.h"
#include "CustomGravityAsyncCallback.h"
#include "EngineUtils.h"
#include "PBDRigidsSolver.h"
#include "GameFramework/Character.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCustomGravityWorldSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UWorld* World = GetWorld())
	{
		// Register delegates to track the ones created/destroyed on the fly
		ActorSpawnedHandle = World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &UCustomGravityWorldSubSystem::AddActorToTrackedCharacters));
		ActorDestroyedHandle = World->AddOnActorDestroyedHandler(FOnActorDestroyed::FDelegate::CreateUObject(this, &UCustomGravityWorldSubSystem::RemoveActorFromTrackedCharacters));
	}
}

void UCustomGravityWorldSubSystem::Deinitialize()
{
	if (UWorld* World = GetWorld())
	{
		World->RemoveOnActorSpawnedHandler(ActorSpawnedHandle);
		World->RemoveOnActorDestroyedHandler(ActorDestroyedHandle);
	}

	TrackedCharacterMovementComponents.Empty();

	Super::Deinitialize();
}

TStatId UCustomGravityWorldSubSystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UCustomGravityWorldSubSystem, STATGROUP_Tickables);
}

void UCustomGravityWorldSubSystem::AddAttractor(UGravityAttractorComponent* GravityAttractorComponent)
{
	Attractors.Add(GravityAttractorComponent);
}

void UCustomGravityWorldSubSystem::RemoveAttractor(UGravityAttractorComponent* GravityAttractorComponent)
{
	Attractors.Remove(GravityAttractorComponent);
}

void UCustomGravityWorldSubSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	if (!IsAsyncCallbackRegistered())
	{
		RegisterAsyncCallback();
	}

	// Find the existing Actors with a Character Movement Component
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AddActorToTrackedCharacters(*ActorItr);
	}
}

void UCustomGravityWorldSubSystem::RegisterAsyncCallback()
{
	if (UWorld* World = GetWorld())
	{
		if (FPhysScene* PhysScene = World->GetPhysicsScene())
		{
			AsyncCallback = PhysScene->GetSolver()->CreateAndRegisterSimCallbackObject_External<FCustomGravityAsyncCallback>();
		}
	}
}

bool UCustomGravityWorldSubSystem::IsAsyncCallbackRegistered() const
{
	return AsyncCallback != nullptr;
}

void UCustomGravityWorldSubSystem::AddGravityAttractorData(const FGravityAttractorData& InputData) const
{
	if (IsAsyncCallbackRegistered())
	{
		FCustomGravityAsyncInput* Input = AsyncCallback->GetProducerInputData_External();
		Input->GravityAttractorsData.Add(InputData);
	}
}

void UCustomGravityWorldSubSystem::UpdateCMCGravities()
{
	for (auto CMComponent : TrackedCharacterMovementComponents)
	{
		// Compute the gravity from all attractors
		FVector AdditionalAcceleration = FVector::ZeroVector;
		for (auto& GravityAttractor : Attractors)
		{
			if (GravityAttractor->ApplyGravity)
			{
				FGravityAttractorData GravityAttractorData = GravityAttractor->GetGravityAttractorData();

				// Direction
				FVector Direction(GravityAttractorData.Location - CMComponent->GetActorLocation());
				double SquaredDistance = FVector::DotProduct(Direction, Direction); // We'll be using UE units here, no meters... 
				Direction.Normalize();

				// Intensity
				double Intensity = GravityAttractorData.MassDotG / SquaredDistance;

				if (FMath::IsNearlyZero(Intensity))
				{
					continue;
				}

				// Add the new acceleration to the force field.  
				AdditionalAcceleration += Intensity * Direction;
			}
		}

		//DrawDebugDirectionalArrow(GetWorld(), CMComponent->GetActorLocation(), CMComponent->GetActorLocation() + AdditionalAcceleration, 1.0, FColor::Red, 0, false, 1.0f  );
		//DrawDebugString(GetWorld(), CMComponent->GetActorLocation(), * FString::Printf(TEXT("%.2f"), AdditionalAcceleration.Length()), nullptr, FColor::Red, 0, false, 1.0f  );

		CMComponent->AddForce(AdditionalAcceleration * CMComponent->Mass);
		CMComponent->SetGravityDirection(AdditionalAcceleration.GetSafeNormal());
	}
}

void UCustomGravityWorldSubSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateCMCGravities();
}

void UCustomGravityWorldSubSystem::AddActorToTrackedCharacters(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	Actor->ForEachComponent<UCharacterMovementComponent>(true, [this](UCharacterMovementComponent* CMComponent)
		{
			if (CMComponent)
			{
				TrackedCharacterMovementComponents.Add(CMComponent);
			}
		});
}

void UCustomGravityWorldSubSystem::RemoveActorFromTrackedCharacters(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	Actor->ForEachComponent<UCharacterMovementComponent>(true, [this](UCharacterMovementComponent* CMComponent)
		{
			if (CMComponent)
			{
				TrackedCharacterMovementComponents.Remove(CMComponent);
			}
		});
}