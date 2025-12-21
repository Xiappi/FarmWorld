// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FarmWorld/UI/Widgets/Inventory/InventoryWidget.h"
#include "Components\WidgetSwitcher.h"
#include "RootWidget.generated.h"

/**
 * 
 */
UCLASS()
class FARMWORLD_API URootWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* WidgetSwitcher;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UInventoryWidget* InventoryWidget;
};
