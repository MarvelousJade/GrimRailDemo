// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "NotebookComponent.h"
#include "CollectibleActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;

/**
 * Base class for collectible items that add entries to the player's notebook
 * Implements the Interactable interface for player interaction
 */
UCLASS(abstract)
class GRIMRAILDEMO_API ACollectibleActor : public AActor, public IInteractable
{
	GENERATED_BODY()

protected:

	/** Visual representation of the collectible */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	/** Collision sphere for interaction detection */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> InteractionSphere;

	/** The notebook entry this collectible adds when picked up */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible")
	FNotebookEntry NotebookEntry;

	/** Text shown as the interaction prompt */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible")
	FText InteractionPromptText;

	/** Whether this collectible can be picked up multiple times */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible")
	bool bCanBeCollectedMultipleTimes = false;

	/** Whether this collectible has already been collected */
	UPROPERTY(BlueprintReadOnly, Category = "Collectible")
	bool bHasBeenCollected = false;

	/** Whether to destroy this actor after collection */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible")
	bool bDestroyOnCollect = true;

	/** Delay before destroying after collection (allows for effects/animations) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible", meta = (ClampMin = 0, ClampMax = 5, Units = "s"))
	float DestroyDelay = 0.1f;

	/** Whether to enable floating/bobbing animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible|Visual")
	bool bEnableFloating = true;

	/** Speed of the floating animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible|Visual", meta = (ClampMin = 0, ClampMax = 10))
	float FloatingSpeed = 1.0f;

	/** Amplitude of the floating animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible|Visual", meta = (ClampMin = 0, ClampMax = 100, Units = "cm"))
	float FloatingAmplitude = 10.0f;

	/** Whether to enable rotation animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible|Visual")
	bool bEnableRotation = true;

	/** Speed of the rotation animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible|Visual", meta = (ClampMin = 0, ClampMax = 500))
	float RotationSpeed = 45.0f;

	/** Starting Z position for floating animation */
	float InitialZPosition = 0.0f;

	/** Current player controller that is focusing on this collectible */
	TObjectPtr<APlayerController> FocusingPlayerController;

public:

	ACollectibleActor();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

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
	 * Called when the collectible is successfully collected
	 * Override this in Blueprint or derived classes for custom behavior
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Collectible", meta = (DisplayName = "On Collected"))
	void BP_OnCollected(APlayerController* Collector);

	/**
	 * Called when the collectible is focused by the player
	 * Override this in Blueprint or derived classes for custom visual effects
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Collectible", meta = (DisplayName = "On Focus Gained"))
	void BP_OnFocusGained(APlayerController* PlayerController);

	/**
	 * Called when the collectible loses focus from the player
	 * Override this in Blueprint or derived classes for custom visual effects
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Collectible", meta = (DisplayName = "On Focus Lost"))
	void BP_OnFocusLost(APlayerController* PlayerController);

	/** Performs the collection logic - adds entry to notebook */
	void PerformCollection(APlayerController* Collector);

	/** Handles visual animations (floating, rotation) */
	void UpdateVisualEffects(float DeltaTime);
};
