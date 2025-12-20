// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemDefinition.h"
#include "InventoryComponent.generated.h"

// Broadcast delegate function for inventory changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FARMWORLD_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UPROPERTY(BlueprintAssignable)
	FOnInventoryChanged OnInventoryChanged;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FInventorySlot> Slots;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxSlots = 40;

	UFUNCTION(BlueprintCallable)
	bool AddItem(UItemDefinition* Item, int32 Amount);

	UFUNCTION(BlueprintCallable)
	bool RemoveItem(UItemDefinition* Item, int32 Amount);
};
