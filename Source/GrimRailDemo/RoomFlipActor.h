// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomFlipActor.generated.h"

/** Current state of the room flip */
UENUM(BlueprintType)
enum class ERoomFlipState : uint8
{
	Idle			UMETA(DisplayName = "Idle"),
	Rotating		UMETA(DisplayName = "Rotating"),
	Completed		UMETA(DisplayName = "Completed")
};

/** Axis around which the room rotates */
UENUM(BlueprintType)
enum class ERoomFlipAxis : uint8
{
	X_Axis			UMETA(DisplayName = "X Axis (Roll)"),
	Y_Axis			UMETA(DisplayName = "Y Axis (Pitch)"),
	Z_Axis			UMETA(DisplayName = "Z Axis (Yaw)")
};

/** Delegate called when room flip starts */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomFlipStarted, ARoomFlipActor*, RoomActor);

/** Delegate called when room flip completes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomFlipCompleted, ARoomFlipActor*, RoomActor);

/** Delegate called each tick during rotation */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRoomFlipProgress, ARoomFlipActor*, RoomActor, float, Progress);

/**
 * Actor that represents a rotatable room/compartment for spatial puzzles
 * Rotates smoothly around a specified axis, maintaining player spatial relationship
 * Core mechanic for GrimRail-style environmental puzzles
 */
UCLASS()
class GRIMRAILDEMO_API ARoomFlipActor : public AActor
{
	GENERATED_BODY()

protected:

	/** Root component that everything attaches to - this rotates */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> RoomRoot;

	/** Current state of the room flip */
	UPROPERTY(BlueprintReadOnly, Category = "Room Flip")
	ERoomFlipState CurrentState = ERoomFlipState::Idle;

	/** Axis around which the room rotates */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Flip")
	ERoomFlipAxis RotationAxis = ERoomFlipAxis::X_Axis;

	/** Angle to rotate (typically 180 for a flip, can be 90, 270, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Flip", meta = (ClampMin = -360, ClampMax = 360, Units = "Degrees"))
	float RotationAngle = 180.0f;

	/** Duration of the rotation in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Flip", meta = (ClampMin = 0.1, ClampMax = 10, Units = "s"))
	float RotationDuration = 3.0f;

	/** Easing curve for rotation (ease in/out for smooth motion) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Flip")
	TObjectPtr<UCurveFloat> RotationCurve;

	/** Whether to attach the player to the room during rotation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Flip")
	bool bAttachPlayer = true;

	/** Whether to disable player input during rotation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Flip")
	bool bDisablePlayerInput = true;

	/** Whether the room can be flipped multiple times */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Flip")
	bool bCanFlipMultipleTimes = false;

	/** Whether to reverse rotation direction each time (creates a toggle effect) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Flip")
	bool bReverseEachFlip = true;

	/** Starting rotation of the room */
	FRotator StartRotation;

	/** Target rotation of the room */
	FRotator TargetRotation;

	/** Current rotation progress (0 to 1) */
	float RotationProgress = 0.0f;

	/** Player pawn attached during rotation */
	TObjectPtr<APawn> AttachedPlayer;

	/** Original player controller for re-enabling input */
	TObjectPtr<APlayerController> PlayerController;

	/** Number of times this room has been flipped */
	int32 FlipCount = 0;

public:

	/** Delegate broadcast when flip starts */
	UPROPERTY(BlueprintAssignable, Category = "Room Flip")
	FOnRoomFlipStarted OnRoomFlipStarted;

	/** Delegate broadcast when flip completes */
	UPROPERTY(BlueprintAssignable, Category = "Room Flip")
	FOnRoomFlipCompleted OnRoomFlipCompleted;

	/** Delegate broadcast during rotation progress */
	UPROPERTY(BlueprintAssignable, Category = "Room Flip")
	FOnRoomFlipProgress OnRoomFlipProgress;

public:

	ARoomFlipActor();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:

	/**
	 * Triggers the room flip sequence
	 * @return True if the flip was started, false if it cannot flip right now
	 */
	UFUNCTION(BlueprintCallable, Category = "Room Flip")
	bool TriggerFlip();

	/**
	 * Checks if the room can currently be flipped
	 * @return True if flip is allowed
	 */
	UFUNCTION(BlueprintPure, Category = "Room Flip")
	bool CanFlip() const;

	/**
	 * Gets the current flip state
	 * @return Current state of the room
	 */
	UFUNCTION(BlueprintPure, Category = "Room Flip")
	ERoomFlipState GetCurrentState() const { return CurrentState; }

	/**
	 * Gets the current rotation progress (0 to 1)
	 * @return Rotation progress
	 */
	UFUNCTION(BlueprintPure, Category = "Room Flip")
	float GetRotationProgress() const { return RotationProgress; }

	/**
	 * Gets the number of times this room has been flipped
	 * @return Flip count
	 */
	UFUNCTION(BlueprintPure, Category = "Room Flip")
	int32 GetFlipCount() const { return FlipCount; }

	/**
	 * Resets the room to its original rotation (for testing/debugging)
	 */
	UFUNCTION(BlueprintCallable, Category = "Room Flip")
	void ResetRoom();

protected:

	/**
	 * Called when the flip starts
	 * Override in Blueprint for custom effects (sounds, particles, etc.)
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Room Flip", meta = (DisplayName = "On Flip Started"))
	void BP_OnFlipStarted();

	/**
	 * Called when the flip completes
	 * Override in Blueprint for custom effects
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Room Flip", meta = (DisplayName = "On Flip Completed"))
	void BP_OnFlipCompleted();

	/**
	 * Called each frame during rotation
	 * Override in Blueprint for custom effects based on progress
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Room Flip", meta = (DisplayName = "On Flip Progress"))
	void BP_OnFlipProgress(float Progress);

	/** Handles the rotation update each tick */
	void UpdateRotation(float DeltaTime);

	/** Attaches the player pawn to the room */
	void AttachPlayerToRoom();

	/** Detaches the player pawn from the room */
	void DetachPlayerFromRoom();

	/** Calculates the target rotation based on axis and angle */
	FRotator CalculateTargetRotation() const;

	/** Gets the rotation axis as a vector */
	FVector GetRotationAxisVector() const;
};
