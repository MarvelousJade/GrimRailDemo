// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HorrorUI.generated.h"

class AHorrorCharacter;

/**
 *  Simple UI for a first person horror game
 *  Manages character sprint meter display
 */
UCLASS(abstract)
class GRIMRAILDEMO_API UHorrorUI : public UUserWidget
{
	GENERATED_BODY()
	
public:

	/** Sets up delegate listeners for the passed character */
	void SetupCharacter(AHorrorCharacter* HorrorCharacter);

	/** Called when the character's sprint meter is updated */
	UFUNCTION()
	void OnSprintMeterUpdated(float Percent);

	/** Called when the character's sprint state changes */
	UFUNCTION()
	void OnSprintStateChanged(bool bSprinting);

	/** Called when an interactable object is detected */
	UFUNCTION()
	void OnInteractableDetected(AActor* InteractableActor, FText InteractionPrompt);

	/** Called when the current interactable is lost */
	UFUNCTION()
	void OnInteractableLost();

protected:

	/** Passes control to Blueprint to update the sprint meter widgets */
	UFUNCTION(BlueprintImplementableEvent, Category="Horror", meta = (DisplayName = "Sprint Meter Updated"))
	void BP_SprintMeterUpdated(float Percent);

	/** Passes control to Blueprint to update the sprint meter status */
	UFUNCTION(BlueprintImplementableEvent, Category="Horror", meta = (DisplayName = "Sprint State Changed"))
	void BP_SprintStateChanged(bool bSprinting);

	/** Passes control to Blueprint to show the interaction prompt */
	UFUNCTION(BlueprintImplementableEvent, Category="Horror", meta = (DisplayName = "Interactable Detected"))
	void BP_InteractableDetected(AActor* InteractableActor, const FText& InteractionPrompt);

	/** Passes control to Blueprint to hide the interaction prompt */
	UFUNCTION(BlueprintImplementableEvent, Category="Horror", meta = (DisplayName = "Interactable Lost"))
	void BP_InteractableLost();
};
