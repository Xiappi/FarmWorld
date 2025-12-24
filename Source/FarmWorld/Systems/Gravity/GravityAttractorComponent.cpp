// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityAttractorComponent.h"
#include "CustomGravityAsyncCallback.h"
#include "CustomGravityWorldSubSystem.h"

// Sets default values for this component's properties
UGravityAttractorComponent::UGravityAttractorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PrePhysics; // Important, we need to be ticked before the Physics Thread to send the attractor location! 
}


// Called when the game starts
void UGravityAttractorComponent::BeginPlay()
{
	Super::BeginPlay();
}

FGravityAttractorData UGravityAttractorComponent::GetGravityAttractorData() const
{
	FGravityAttractorData GravityAttractorData;
	GravityAttractorData.Location = GetComponentLocation();
	if (bUseGravityAtRadius)
	{
		GravityAttractorData.MassDotG = Gravity * Radius * Radius;
	}
	else
	{
		GravityAttractorData.MassDotG = Mass * 6.67430E-5; // G = 6.67430E-11 m³kg⁻¹s⁻², because 1m equals 100 UE Units, we have to multiply by a 100³ factor, so E-11 goes E-5 
	}

	return GravityAttractorData;
}

void UGravityAttractorComponent::BuildAsyncInput()
{
	if (ApplyGravity) // Publish attractor data only if active
	{
		if (UWorld* World = GetWorld())
		{
			if (UCustomGravityWorldSubSystem* GravitySubsystem = World->GetSubsystem<UCustomGravityWorldSubSystem>())
			{
				GravitySubsystem->AddGravityAttractorData(GetGravityAttractorData());
			}
		}
	}
}

// Called every frame
void UGravityAttractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	BuildAsyncInput();

	DrawDebugSphere(GetWorld(), GetComponentLocation(), Radius, 12, FColor::Red, false, -1.0f, 0, 2.0f);
}

void UGravityAttractorComponent::OnRegister()
{
	Super::OnRegister();

	if (UWorld* World = GetWorld())
	{
		if (UCustomGravityWorldSubSystem* GravitySubsystem = World->GetSubsystem<UCustomGravityWorldSubSystem>())
		{
			GravitySubsystem->AddAttractor(this);
		}
	}
}

void UGravityAttractorComponent::OnUnregister()
{
	if (UWorld* World = GetWorld())
	{
		if (UCustomGravityWorldSubSystem* GravitySubsystem = World->GetSubsystem<UCustomGravityWorldSubSystem>())
		{
			GravitySubsystem->RemoveAttractor(this);
		}
	}

	Super::OnUnregister();
}

