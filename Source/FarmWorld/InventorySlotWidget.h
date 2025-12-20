// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components\SizeBox.h"
#include "Blueprint\WidgetTree.h"
#include "Components\Overlay.h"
#include "Components\Image.h"
#include "Components\TextBlock.h"
#include "ItemDefinition.h"
#include "InventorySlotWidget.generated.h"


/**
 * 
 */
UCLASS()
class FARMWORLD_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetupSlot(UItemDefinition* Item, int32 Quantity, int32 InSlotIndex);

public:
	UPROPERTY(meta = (BindWidget))
	USizeBox* SizeBox;

	UPROPERTY(meta = (BindWidget))
	UOverlay* RootOverlay;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemQuantityText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* EmptySlotImage;

	int32 SlotIndex = INDEX_NONE;

protected:
	virtual void NativeConstruct() override;
};
