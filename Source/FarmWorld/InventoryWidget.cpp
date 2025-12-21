// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"

void UInventoryWidget::RebuildInventory()
{
	if(!Inventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory is not set."));
		return;
	}

	if (!ItemGrid)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemGrid is not set."));
		return;
	}

	if (!SlotWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("SlotWidgetClass is not set."));
		return;
	}

	ItemGrid->ClearChildren();

	// Fix the CreateWidget call to use SlotWidgetClass instead of the type name
	const TArray<FInventorySlot>& Slots = Inventory->Slots;
	const int32 MaxSlots = Inventory->MaxSlots;

	UE_LOG(LogTemp, Log, TEXT("Rebuilding inventory with %d slots."), MaxSlots);
	for (int32 i = 0; i < MaxSlots; ++i)
	{
		// This is a copy of the slot or a default empty slot
		FInventorySlot CurrentSlot = Slots.IsValidIndex(i) ? Slots[i] : FInventorySlot{ nullptr, 0, i };

		UInventorySlotWidget* SlotWidget =
			CreateWidget<UInventorySlotWidget>(this, SlotWidgetClass);

		if (!SlotWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create SlotWidget for index %d."), i);
			continue;
		}

		const int32 Row = i / Columns;
		const int32 Col = i % Columns;

		SlotWidget->SetupSlot(CurrentSlot.Item, CurrentSlot.Quantity, i);
		ItemGrid->AddChildToUniformGrid(SlotWidget, Row, Col);
	}
}

void UInventoryWidget::SetInventory(UInventoryComponent* InInventory)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Setting inventory in InventoryWidget."));
	Inventory = InInventory;
	RebuildInventory();

	Inventory->OnInventoryChanged.AddDynamic(
		this,
		&UInventoryWidget::RebuildInventory
	);
}
