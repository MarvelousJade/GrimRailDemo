// Copyright Epic Games, Inc. All Rights Reserved.

#include "RoomFlipActor.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Curves/CurveFloat.h"
#include "Kismet/GameplayStatics.h"

ARoomFlipActor::ARoomFlipActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create root component for rotation
	RoomRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RoomRoot"));
	RootComponent = RoomRoot;

	// Default values
	CurrentState = ERoomFlipState::Idle;
	RotationProgress = 0.0f;
	AttachedPlayer = nullptr;
	PlayerController = nullptr;
	FlipCount = 0;
}

void ARoomFlipActor::BeginPlay()
{
	Super::BeginPlay();

	// Store the initial rotation
	StartRotation = RoomRoot->GetComponentRotation();
}

void ARoomFlipActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == ERoomFlipState::Rotating)
	{
		UpdateRotation(DeltaTime);
	}
}

bool ARoomFlipActor::TriggerFlip()
{
	if (!CanFlip())
	{
		UE_LOG(LogTemp, Warning, TEXT("RoomFlipActor: Cannot flip right now"));
		return false;
	}

	// Set state to rotating
	CurrentState = ERoomFlipState::Rotating;
	RotationProgress = 0.0f;
	FlipCount++;

	// Store starting rotation
	StartRotation = RoomRoot->GetComponentRotation();

	// Calculate target rotation
	TargetRotation = CalculateTargetRotation();

	// Attach player if configured
	if (bAttachPlayer)
	{
		AttachPlayerToRoom();
	}

	// Disable player input if configured
	if (bDisablePlayerInput && PlayerController)
	{
		PlayerController->DisableInput(PlayerController);
	}

	// Broadcast started event
	OnRoomFlipStarted.Broadcast(this);
	BP_OnFlipStarted();

	UE_LOG(LogTemp, Log, TEXT("RoomFlipActor: Flip started (Flip #%d)"), FlipCount);

	return true;
}

bool ARoomFlipActor::CanFlip() const
{
	// Cannot flip if already rotating
	if (CurrentState == ERoomFlipState::Rotating)
	{
		return false;
	}

	// Check if multiple flips allowed
	if (CurrentState == ERoomFlipState::Completed && !bCanFlipMultipleTimes)
	{
		return false;
	}

	return true;
}

void ARoomFlipActor::ResetRoom()
{
	CurrentState = ERoomFlipState::Idle;
	RotationProgress = 0.0f;
	FlipCount = 0;

	// Reset rotation to original
	RoomRoot->SetWorldRotation(StartRotation);

	// Detach player if attached
	if (AttachedPlayer)
	{
		DetachPlayerFromRoom();
	}

	UE_LOG(LogTemp, Log, TEXT("RoomFlipActor: Room reset to initial state"));
}

void ARoomFlipActor::UpdateRotation(float DeltaTime)
{
	// Update progress
	RotationProgress += DeltaTime / RotationDuration;
	RotationProgress = FMath::Clamp(RotationProgress, 0.0f, 1.0f);

	// Apply easing curve if available
	float EasedProgress = RotationProgress;
	if (RotationCurve)
	{
		EasedProgress = RotationCurve->GetFloatValue(RotationProgress);
	}
	else
	{
		// Default smoothstep easing if no curve provided
		EasedProgress = FMath::SmoothStep(0.0f, 1.0f, RotationProgress);
	}

	// Interpolate rotation
	FRotator CurrentRotation = FMath::Lerp(StartRotation, TargetRotation, EasedProgress);
	RoomRoot->SetWorldRotation(CurrentRotation);

	// Broadcast progress event
	OnRoomFlipProgress.Broadcast(this, RotationProgress);
	BP_OnFlipProgress(RotationProgress);

	// Check if rotation completed
	if (RotationProgress >= 1.0f)
	{
		CurrentState = ERoomFlipState::Completed;

		// Detach player
		if (AttachedPlayer)
		{
			DetachPlayerFromRoom();
		}

		// Re-enable player input
		if (bDisablePlayerInput && PlayerController)
		{
			PlayerController->EnableInput(PlayerController);
		}

		// Broadcast completed event
		OnRoomFlipCompleted.Broadcast(this);
		BP_OnFlipCompleted();

		// If can flip multiple times and reversing, prepare for next flip
		if (bCanFlipMultipleTimes)
		{
			CurrentState = ERoomFlipState::Idle;

			// Reverse rotation angle for toggle effect
			if (bReverseEachFlip)
			{
				RotationAngle = -RotationAngle;
			}
		}

		UE_LOG(LogTemp, Log, TEXT("RoomFlipActor: Flip completed"));
	}
}

void ARoomFlipActor::AttachPlayerToRoom()
{
	// Get player pawn
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("RoomFlipActor: No player controller found"));
		return;
	}

	AttachedPlayer = PlayerController->GetPawn();
	if (!AttachedPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("RoomFlipActor: No player pawn found"));
		return;
	}

	// Attach pawn to room root with KeepWorld rules to maintain current position
	AttachedPlayer->AttachToComponent(
		RoomRoot,
		FAttachmentTransformRules::KeepWorldTransform
	);

	UE_LOG(LogTemp, Log, TEXT("RoomFlipActor: Player attached to room"));
}

void ARoomFlipActor::DetachPlayerFromRoom()
{
	if (AttachedPlayer)
	{
		// Store current location
		FVector CurrentLocation = AttachedPlayer->GetActorLocation();

		// Detach player
		AttachedPlayer->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// Reset player rotation to upright (only keep yaw for facing direction)
		FRotator CurrentRotation = AttachedPlayer->GetActorRotation();
		FRotator UprightRotation = FRotator(0.0f, CurrentRotation.Yaw, 0.0f);
		AttachedPlayer->SetActorRotation(UprightRotation);

		// Ensure player stays at current location
		AttachedPlayer->SetActorLocation(CurrentLocation);

		AttachedPlayer = nullptr;
		UE_LOG(LogTemp, Log, TEXT("RoomFlipActor: Player detached from room and reset to upright"));
	}

	PlayerController = nullptr;
}

FRotator ARoomFlipActor::CalculateTargetRotation() const
{
	FRotator Target = StartRotation;

	switch (RotationAxis)
	{
	case ERoomFlipAxis::X_Axis:
		Target.Roll += RotationAngle;
		break;
	case ERoomFlipAxis::Y_Axis:
		Target.Pitch += RotationAngle;
		break;
	case ERoomFlipAxis::Z_Axis:
		Target.Yaw += RotationAngle;
		break;
	}

	return Target;
}

FVector ARoomFlipActor::GetRotationAxisVector() const
{
	switch (RotationAxis)
	{
	case ERoomFlipAxis::X_Axis:
		return FVector::ForwardVector;
	case ERoomFlipAxis::Y_Axis:
		return FVector::RightVector;
	case ERoomFlipAxis::Z_Axis:
		return FVector::UpVector;
	default:
		return FVector::ForwardVector;
	}
}
