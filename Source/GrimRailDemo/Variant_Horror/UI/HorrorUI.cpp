// Copyright Epic Games, Inc. All Rights Reserved.


#include "HorrorUI.h"
#include "HorrorCharacter.h"

void UHorrorUI::SetupCharacter(AHorrorCharacter* HorrorCharacter)
{
	HorrorCharacter->OnSprintMeterUpdated.AddDynamic(this, &UHorrorUI::OnSprintMeterUpdated);
	HorrorCharacter->OnSprintStateChanged.AddDynamic(this, &UHorrorUI::OnSprintStateChanged);
	HorrorCharacter->OnInteractableDetected.AddDynamic(this, &UHorrorUI::OnInteractableDetected);
	HorrorCharacter->OnInteractableLost.AddDynamic(this, &UHorrorUI::OnInteractableLost);
}

void UHorrorUI::OnSprintMeterUpdated(float Percent)
{
	// call the BP handler
	BP_SprintMeterUpdated(Percent);
}

void UHorrorUI::OnSprintStateChanged(bool bSprinting)
{
	// call the BP handler
	BP_SprintStateChanged(bSprinting);
}

void UHorrorUI::OnInteractableDetected(AActor* InteractableActor, FText InteractionPrompt)
{
	// call the BP handler
	BP_InteractableDetected(InteractableActor, InteractionPrompt);
}

void UHorrorUI::OnInteractableLost()
{
	// call the BP handler
	BP_InteractableLost();
}
