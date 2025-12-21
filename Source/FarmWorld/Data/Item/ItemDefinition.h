// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDefinition.generated.h"

USTRUCT(BlueprintType)
struct FItemData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* Icon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bStackable = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 MaxStackSize = 99;
};

USTRUCT(BlueprintType)
struct FInventorySlot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    UItemDefinition* Item;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    int32 Quantity;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    int32 SlotIndex;
};

UCLASS(BlueprintType)
class FARMWORLD_API UItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FItemData Item;
};
