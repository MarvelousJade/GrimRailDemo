// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "InteractableTrigger.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

/**
 * Base class for simple interactable triggers (levers, buttons, switches)
 * Provides a simple interface for Blueprint subclasses to respond to player interaction
 */
UCLASS(abstract)
class GRIMRAILDEMO_API AInteractableTrigger : public AActor, public IInteractable
{
	GENERATED_BODY()

protected:

	/** Visual mesh for the trigger */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> TriggerMesh;

	/** Collision component for interaction detection */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> InteractionBox;

	/** Text shown as the interaction prompt */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText InteractionPromptText;

	/** Whether this trigger can currently be interacted with */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bCanInteract = true;

	/** Whether this trigger can only be used once */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bSingleUse = false;

	/** Whether this trigger has already been used */
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	bool bHasBeenUsed = false;

public:

	AInteractableTrigger();

protected:

	virtual void BeginPlay() override;

public:

	//~Begin IInteractable Interface
	virtual void OnInteractionFocus_Implementation(APlayerController* PlayerController) override;
	virtual void OnInteractionFocusLost_Implementation(APlayerController* PlayerController) override;
	virtual bool OnInteract_Implementation(APlayerController* PlayerController) override;
	virtual FText GetInteractionPrompt_Implementation() const override;
	virtual bool CanInteract_Implementation(APlayerController* PlayerController) const override;
	//~End IInteractable Interface

protected:

	/**
	 * Called when the player successfully interacts with this trigger
	 * Override this in Blueprint or derived classes for custom behavior
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction", meta = (DisplayName = "On Triggered"))
	void BP_OnTriggered(APlayerController* PlayerController);

	/**
	 * Called when the player looks at this trigger
	 * Override this in Blueprint for custom visual effects
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction", meta = (DisplayName = "On Focus Gained"))
	void BP_OnFocusGained(APlayerController* PlayerController);

	/**
	 * Called when the player stops looking at this trigger
	 * Override this in Blueprint for custom visual effects
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction", meta = (DisplayName = "On Focus Lost"))
	void BP_OnFocusLost(APlayerController* PlayerController);
};
