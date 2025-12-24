// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Systems\Gravity\GravityAttractorComponent.h>
#include "Planet.generated.h"

UCLASS()
class FARMWORLD_API APlanet : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlanet();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UGravityAttractorComponent* GravityAttractor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Planet", meta = (AllowPrivateAccess = "true"))
	float RotationSpeed = 10.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
