// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{

}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UInventoryComponent::AddItem(UItemDefinition* Item, int32 Amount)
{
    if (!Item)
    {
        return false;
    }

    // Try stacking first
    if (Item->Item.bStackable)
    {
        for (auto& Slot : Slots)
        {
            if (Slot.Item == Item && Slot.Quantity < Item->Item.MaxStackSize)
            {
                int32 Space = Item->Item.MaxStackSize - Slot.Quantity;
                int32 ToAdd = FMath::Min(Space, Amount);
                Slot.Quantity += ToAdd;
                Amount -= ToAdd;

                if (Amount <= 0)
                {
                    return true;
                }
            }
        }
    }

    // Add new slots if space remains
    while (Amount > 0 && Slots.Num() < MaxSlots)
    {
        int32 ToAdd = Item->Item.bStackable
            ? FMath::Min(Item->Item.MaxStackSize, Amount)
            : 1;

        Slots.Add({ Item, ToAdd });
        Amount -= ToAdd;
    }

    OnInventoryChanged.Broadcast();
    return Amount == 0;
}

bool UInventoryComponent::RemoveItem(UItemDefinition* Item, int32 Amount)
{
    if (!Item || Amount <= 0)
    {
        return false;
    }

    for (int32 i = Slots.Num() - 1; i >= 0 && Amount > 0; --i)
    {
        if (Slots[i].Item == Item)
        {
            int32 ToRemove = FMath::Min(Slots[i].Quantity, Amount);
            Slots[i].Quantity -= ToRemove;
            Amount -= ToRemove;

            if (Slots[i].Quantity <= 0)
            {
                Slots.RemoveAt(i);
            }
        }
    }

    OnInventoryChanged.Broadcast();
    return Amount <= 0;
}