// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"

void UInventoryWidget::RebuildInventory(const TArray<FInventorySlot>& Slots, int32 MaxSlots)
{

	if (!ItemGrid || !SlotWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemGrid or SlotWidgetClass is not set."));
	}

	ItemGrid->ClearChildren();

	// Fix the CreateWidget call to use SlotWidgetClass instead of the type name
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
