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
		ActorSpawnedHandle = World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &UCustomGravityWorldSubSystem::AddGravityConsumer));
		ActorDestroyedHandle = World->AddOnActorDestroyedHandler(FOnActorDestroyed::FDelegate::CreateUObject(this, &UCustomGravityWorldSubSystem::RemoveGravityConsumer));
	}
}

void UCustomGravityWorldSubSystem::Deinitialize()
{
	if (UWorld* World = GetWorld())
	{
		World->RemoveOnActorSpawnedHandler(ActorSpawnedHandle);
		World->RemoveOnActorDestroyedHandler(ActorDestroyedHandle);
	}

	GravityConsumers.Empty();

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
		AddGravityConsumer(*ActorItr);
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

void UCustomGravityWorldSubSystem::HandleGravityConsumers(float DeltaTime)
{
	GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Green, FString::Printf(TEXT("Gravity Consumers: %d"), GravityConsumers.Num()));
	for (auto GravityConsumerObj : GravityConsumers)
	{
		if (!GravityConsumerObj.IsValid())
		{
			continue;
		}

		IGravityConsumer* Consumer = Cast<IGravityConsumer>(GravityConsumerObj.Get());

		if (Consumer && Consumer->IsGravityEnabled())
		{
			// Compute the gravity from all attractors
			FVector SampleLocation = Consumer->GetGravitySampleLocation();
			FVector TotalGravity = ComputeGravityAtLocation(SampleLocation);

			// Apply the computed gravity to the consumer
			Consumer->ApplyGravity(TotalGravity.GetSafeNormal(), TotalGravity.Size(), DeltaTime);
		}
	}
}

FVector UCustomGravityWorldSubSystem::ComputeGravityAtLocation(const FVector& SampleLocation)
{
	FVector TotalGravity = FVector::ZeroVector;
	for (auto& GravityAttractor : Attractors)
	{
		if (GravityAttractor->ApplyGravity)
		{
			FGravityAttractorData GravityAttractorData = GravityAttractor->GetGravityAttractorData();

			// Direction
			FVector Direction(GravityAttractorData.Location - SampleLocation);
			double SquaredDistance = FVector::DotProduct(Direction, Direction); // We'll be using UE units here, no meters... 
			Direction.Normalize();

			// Intensity
			double Intensity = GravityAttractorData.MassDotG / SquaredDistance;

			if (FMath::IsNearlyZero(Intensity))
			{
				continue;
			}

			// Add to total gravity
			TotalGravity += Intensity * Direction;
		}
	}

	return TotalGravity;
}

void UCustomGravityWorldSubSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HandleGravityConsumers(DeltaTime);
}

void UCustomGravityWorldSubSystem::AddGravityConsumer(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	if (Actor->GetClass()->ImplementsInterface(UGravityConsumer::StaticClass()))
	{
		GravityConsumers.Add(Actor);
	}
}

void UCustomGravityWorldSubSystem::RemoveGravityConsumer(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	if (Actor->GetClass()->ImplementsInterface(UGravityConsumer::StaticClass()))
	{
		GravityConsumers.Remove(Actor);
	}
}