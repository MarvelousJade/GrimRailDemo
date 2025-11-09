// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for actors that can be interacted with by the player
 * Provides raycast-based interaction system for pickups, triggers, etc.
 */
class GRIMRAILDEMO_API IInteractable
{
	GENERATED_BODY()

public:

	/**
	 * Called when the player looks at this interactable object
	 * Use this to show interaction prompts or highlight effects
	 * @param PlayerController The player controller looking at this object
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteractionFocus(APlayerController* PlayerController);

	/**
	 * Called when the player stops looking at this interactable object
	 * Use this to hide interaction prompts or remove highlight effects
	 * @param PlayerController The player controller that was looking at this object
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteractionFocusLost(APlayerController* PlayerController);

	/**
	 * Called when the player interacts with this object (e.g., pressing E)
	 * @param PlayerController The player controller performing the interaction
	 * @return True if the interaction was successful
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool OnInteract(APlayerController* PlayerController);

	/**
	 * Gets the text to display as the interaction prompt
	 * @return The prompt text (e.g., "Press E to pick up", "Examine")
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractionPrompt() const;

	/**
	 * Checks if this interactable can currently be interacted with
	 * @param PlayerController The player controller attempting interaction
	 * @return True if interaction is allowed
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool CanInteract(APlayerController* PlayerController) const;
};
