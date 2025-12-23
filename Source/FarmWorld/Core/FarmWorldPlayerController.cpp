// Copyright Epic Games, Inc. All Rights Reserved.


#include "FarmWorldPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "FarmWorld.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include <Characters\Player\FarmWorldCharacter.h>

void AFarmWorldPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);
		}
		else
		{
			UE_LOG(LogFarmWorld, Error, TEXT("Could not spawn mobile controls widget."));
		}

	}
}

void AFarmWorldPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

bool AFarmWorldPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

void AFarmWorldPlayerController::ToggleRootUI()
{
	UE_LOG(LogFarmWorld, Log, TEXT("Toggling Root UI"));
	if (!IsLocalPlayerController())
	{
		return;
	}

	if (bRootMenuVisible)
	{
		HideRootUI();
		return;
	}

	ShowRootUI();
}

void AFarmWorldPlayerController::ShowRootUI()
{
	UE_LOG(LogFarmWorld, Log, TEXT("Showing Root UI"));
	if (!RootWidget)
	{
		if (!RootWidgetClass)
		{
			UE_LOG(LogFarmWorld, Error, TEXT("RootWidgetClass not set on PlayerController."));
			return;
		}

		RootWidget = CreateWidget<URootWidget>(this, RootWidgetClass);
		if (AFarmWorldCharacter* FarmWorldCharacter = Cast<AFarmWorldCharacter>(GetPawn()))
		{
			UE_LOG(LogFarmWorld, Log, TEXT("Setting Inventory on RootWidget."));
			if (UInventoryComponent* Inventory = FarmWorldCharacter->Inventory)
			{
				UE_LOG(LogFarmWorld, Log, TEXT("Inventory found, setting on RootWidget."));
				RootWidget->InventoryWidget->SetInventory(Inventory);
			}
		}

		if (!RootWidget)
		{
			UE_LOG(LogFarmWorld, Error, TEXT("Failed to create RootWidget."));
			return;
		}
	}

	if (!RootWidget->IsInViewport())
	{
		RootWidget->AddToViewport(0);
	}

	bShowMouseCursor = true;

	FInputModeGameAndUI Mode;
	Mode.SetWidgetToFocus(RootWidget->TakeWidget());
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(Mode);

	bRootMenuVisible = true;
}

void AFarmWorldPlayerController::HideRootUI()
{
	UE_LOG(LogFarmWorld, Log, TEXT("Hiding Root UI"));
	if (RootWidget && RootWidget->IsInViewport())
	{
		RootWidget->RemoveFromParent();
	}

	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());

	bRootMenuVisible = false;
}
