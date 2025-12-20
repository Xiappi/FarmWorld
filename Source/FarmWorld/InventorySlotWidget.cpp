// Fill out your copyright notice in the Description page of Project Settings.

#include "InventorySlotWidget.h"

//UInventorySlotWidget::UInventorySlotWidget(const FObjectInitializer& ObjectInitializer)	: Super(ObjectInitializer)
//{
//	//if (WidgetTree)
//	//{
//	//	SizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("SizeBox"));
//	//	RootOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("RootOverlay"));
//	//	ItemImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("ItemImage"));
//	//	ItemQuantityText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ItemQuantityText"));
//
//	//	WidgetTree->RootWidget = SizeBox;
//	//	SizeBox->AddChild(RootOverlay);
//	//	RootOverlay->AddChild(ItemImage);
//	//	RootOverlay->AddChild(ItemQuantityText);
//	//}
//}

void UInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UE_LOG(LogTemp, Warning, TEXT("NATIVE CONSTRUCT"));


	if (RootOverlay)
	{
		UE_LOG(LogTemp, Warning, TEXT("RootOverlay is bound successfully."));
	}

	if (SizeBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("SizeBox is bound successfully."));
	}

	if (ItemImage)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemImage is bound successfully."));
	}

	if (ItemQuantityText)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemQuantityText is bound successfully."));
	}
}

void UInventorySlotWidget::SetupSlot(UItemDefinition* Item, int32 Quantity, int32 InSlotIndex)
{
	SlotIndex = InSlotIndex;

	// Empty slot
	if (!Item)
	{
		ItemImage->SetBrushFromTexture(EmptySlotImage, true);
		ItemQuantityText->SetVisibility(ESlateVisibility::Hidden);
		return;
	}


	// Valid item
	ItemImage->SetBrushFromTexture(Item->Item.Icon, true);

	if (Quantity > 1 && Item->Item.bStackable)
	{
		ItemQuantityText->SetText(FText::AsNumber(Quantity));
		ItemQuantityText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ItemQuantityText->SetText(FText::GetEmpty());
		ItemQuantityText->SetVisibility(ESlateVisibility::Hidden);
	}
}

