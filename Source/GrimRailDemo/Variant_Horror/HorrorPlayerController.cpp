// Copyright Epic Games, Inc. All Rights Reserved.


#include "Variant_Horror/HorrorPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "GrimRailDemoCameraManager.h"
#include "HorrorCharacter.h"
#include "HorrorUI.h"
#include "GrimRailDemo.h"
#include "Widgets/Input/SVirtualJoystick.h"

AHorrorPlayerController::AHorrorPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = AGrimRailDemoCameraManager::StaticClass();
}

void AHorrorPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogGrimRailDemo, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

void AHorrorPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	// only spawn UI on local player controllers
	if (IsLocalPlayerController())
	{
		// set up the UI for the character
		if (AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(aPawn))
		{
			// create the HUD UI
			if (!HorrorUI)
			{
				HorrorUI = CreateWidget<UHorrorUI>(this, HorrorUIClass);
				HorrorUI->AddToViewport(0);
			}

			HorrorUI->SetupCharacter(HorrorCharacter);

			// create the notebook UI
			if (NotebookWidgetClass && !NotebookWidget)
			{
				NotebookWidget = CreateWidget<UUserWidget>(this, NotebookWidgetClass);
				if (NotebookWidget)
				{
					NotebookWidget->AddToViewport(10); // Higher Z-order than HUD
					NotebookWidget->SetVisibility(ESlateVisibility::Hidden);

					// Bind to notebook toggle delegate
					HorrorCharacter->OnNotebookToggled.AddDynamic(this, &AHorrorPlayerController::OnNotebookToggled);
				}
			}
		}
	}

}

void AHorrorPlayerController::SetupInputComponent()
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
			if (!SVirtualJoystick::ShouldDisplayTouchInterface())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

void AHorrorPlayerController::OnNotebookToggled(bool bIsOpen)
{
	if (NotebookWidget)
	{
		if (bIsOpen)
		{
			NotebookWidget->SetVisibility(ESlateVisibility::Visible);

			// Show mouse cursor and enable UI mode
			SetShowMouseCursor(true);
			SetInputMode(FInputModeGameAndUI());
		}
		else
		{
			NotebookWidget->SetVisibility(ESlateVisibility::Hidden);

			// Hide mouse cursor and return to game mode
			SetShowMouseCursor(false);
			SetInputMode(FInputModeGameOnly());
		}

		UE_LOG(LogTemp, Log, TEXT("HorrorPlayerController: Notebook toggled %s"), bIsOpen ? TEXT("open") : TEXT("closed"));
	}
}
