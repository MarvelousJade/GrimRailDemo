// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GrimRailDemoCharacter.h"
#include "HorrorCharacter.generated.h"

class USpotLightComponent;
class UInputAction;
class UNotebookComponent;
class IInteractable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateSprintMeterDelegate, float, Percentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSprintStateChangedDelegate, bool, bSprinting);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractableDetected, AActor*, InteractableActor, FText, InteractionPrompt);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractableLost);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotebookToggled, bool, bIsOpen);

/**
 *  Simple first person horror character
 *  Provides stamina-based sprinting
 */
UCLASS(abstract)
class GRIMRAILDEMO_API AHorrorCharacter : public AGrimRailDemoCharacter
{
	GENERATED_BODY()

	/** Player light source */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpotLightComponent* SpotLight;

	/** Notebook component for tracking clues and objectives */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNotebookComponent> NotebookComponent;

protected:

	/** Sprint input action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* SprintAction;

	/** Interact input action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* InteractAction;

	/** Toggle notebook input action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* ToggleNotebookAction;

	/** If true, we're sprinting */
	bool bSprinting = false;

	/** If true, we're recovering stamina */
	bool bRecovering = false;

	/** Default walk speed when not sprinting or recovering */
	UPROPERTY(EditAnywhere, Category="Walk")
	float WalkSpeed = 250.0f;

	/** Time interval for sprinting stamina ticks */
	UPROPERTY(EditAnywhere, Category="Sprint", meta = (ClampMin = 0, ClampMax = 1, Units = "s"))
	float SprintFixedTickTime = 0.03333f;

	/** Sprint stamina amount. Maxes at SprintTime */
	float SprintMeter = 0.0f;

	/** How long we can sprint for, in seconds */
	UPROPERTY(EditAnywhere, Category="Sprint", meta = (ClampMin = 0, ClampMax = 10, Units = "s"))
	float SprintTime = 3.0f;

	/** Walk speed while sprinting */
	UPROPERTY(EditAnywhere, Category="Sprint", meta = (ClampMin = 0, ClampMax = 10, Units = "cm/s"))
	float SprintSpeed = 600.0f;

	/** Walk speed while recovering stamina */
	UPROPERTY(EditAnywhere, Category="Recovery", meta = (ClampMin = 0, ClampMax = 10, Units = "cm/s"))
	float RecoveringWalkSpeed = 150.0f;

	/** Time it takes for the sprint meter to recover */
	UPROPERTY(EditAnywhere, Category="Recovery", meta = (ClampMin = 0, ClampMax = 10, Units = "s"))
	float RecoveryTime = 0.0f;

	/** Sprint tick timer */
	FTimerHandle SprintTimer;

	/** Max distance for interaction raycasts */
	UPROPERTY(EditAnywhere, Category="Interaction", meta = (ClampMin = 0, ClampMax = 1000, Units = "cm"))
	float InteractionDistance = 120.0f;

	/** How often to check for interactable objects (seconds) */
	UPROPERTY(EditAnywhere, Category="Interaction", meta = (ClampMin = 0, ClampMax = 1, Units = "s"))
	float InteractionCheckRate = 0.1f;

	/** Whether the notebook is currently open */
	bool bIsNotebookOpen = false;

	/** Currently focused interactable actor */
	TObjectPtr<AActor> CurrentInteractable;

	/** Timer for checking interactables */
	FTimerHandle InteractionCheckTimer;

public:

	/** Delegate called when the sprint meter should be updated */
	FUpdateSprintMeterDelegate OnSprintMeterUpdated;

	/** Delegate called when we start and stop sprinting */
	FSprintStateChangedDelegate OnSprintStateChanged;

	/** Delegate called when an interactable object is detected */
	FOnInteractableDetected OnInteractableDetected;

	/** Delegate called when the current interactable is lost */
	FOnInteractableLost OnInteractableLost;

	/** Delegate called when the notebook is opened or closed */
	FOnNotebookToggled OnNotebookToggled;

protected:

	/** Constructor */
	AHorrorCharacter();

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Gameplay cleanup */
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

protected:

	/** Starts sprinting behavior */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoStartSprint();

	/** Stops sprinting behavior */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoEndSprint();

	/** Called while sprinting at a fixed time interval */
	void SprintFixedTick();

	/** Handles interact button press */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoInteract();

	/** Handles toggle notebook button press */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoToggleNotebook();

	/** Checks for interactable objects in front of the player */
	void CheckForInteractables();

	/** Sets the currently focused interactable */
	void SetCurrentInteractable(AActor* NewInteractable);

public:

	/** Gets the notebook component */
	UFUNCTION(BlueprintPure, Category="Notebook")
	UNotebookComponent* GetNotebookComponent() const { return NotebookComponent; }

	/** Checks if the notebook is currently open */
	UFUNCTION(BlueprintPure, Category="Notebook")
	bool IsNotebookOpen() const { return bIsNotebookOpen; }
};
