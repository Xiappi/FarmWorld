// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FarmWorld/Data/Item/ItemDefinition.h"
#include "Components/UniformGridPanel.h"
#include "InventorySlotWidget.h"
#include "FarmWorld/Components/Inventory/InventoryComponent.h"
#include "InventoryWidget.generated.h"


UCLASS()
class FARMWORLD_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void RebuildInventory();

    UFUNCTION(BlueprintCallable)
    void SetInventory(UInventoryComponent* InInventory);

protected:
    UPROPERTY(meta = (BindWidget))
    UUniformGridPanel* ItemGrid;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

    UPROPERTY(EditDefaultsOnly)
    int32 Columns = 10;

    UInventoryComponent* Inventory = nullptr;
};
