// Copyright Epic Games, Inc. All Rights Reserved.


#include "Variant_Horror/HorrorCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "NotebookComponent.h"
#include "Interactable.h"
#include "DrawDebugHelpers.h"

AHorrorCharacter::AHorrorCharacter()
{
	// create the spotlight
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(GetFirstPersonCameraComponent());

	SpotLight->SetRelativeLocationAndRotation(FVector(30.0f, 17.5f, -5.0f), FRotator(-18.6f, -1.3f, 5.26f));
	SpotLight->Intensity = 0.5;
	SpotLight->SetIntensityUnits(ELightUnits::Lumens);
	SpotLight->AttenuationRadius = 1050.0f;
	SpotLight->InnerConeAngle = 18.7f;
	SpotLight->OuterConeAngle = 45.24f;

	// create the notebook component
	NotebookComponent = CreateDefaultSubobject<UNotebookComponent>(TEXT("NotebookComponent"));
}

void AHorrorCharacter::BeginPlay()
{
	Super::BeginPlay();

	// initialize sprint meter to max
	SprintMeter = SprintTime;

	// Initialize the walk speed
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// start the sprint tick timer
	GetWorld()->GetTimerManager().SetTimer(SprintTimer, this, &AHorrorCharacter::SprintFixedTick, SprintFixedTickTime, true);

	// start the interaction check timer
	GetWorld()->GetTimerManager().SetTimer(InteractionCheckTimer, this, &AHorrorCharacter::CheckForInteractables, InteractionCheckRate, true);
}

void AHorrorCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// clear the sprint timer
	GetWorld()->GetTimerManager().ClearTimer(SprintTimer);

	// clear the interaction check timer
	GetWorld()->GetTimerManager().ClearTimer(InteractionCheckTimer);
}

void AHorrorCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	{
		// Set up action bindings
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
		{
			// Sprinting
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AHorrorCharacter::DoStartSprint);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AHorrorCharacter::DoEndSprint);

			// Interaction
			if (InteractAction)
			{
				EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AHorrorCharacter::DoInteract);
			}

			// Notebook
			if (ToggleNotebookAction)
			{
				EnhancedInputComponent->BindAction(ToggleNotebookAction, ETriggerEvent::Triggered, this, &AHorrorCharacter::DoToggleNotebook);
			}
		}
	}
}

void AHorrorCharacter::DoStartSprint()
{
	// set the sprinting flag
	bSprinting = true;

	// are we out of recovery mode?
	if (!bRecovering)
	{
		// set the sprint walk speed
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

		// call the sprint state changed delegate
		OnSprintStateChanged.Broadcast(true);
	}

}

void AHorrorCharacter::DoEndSprint()
{
	// set the sprinting flag
	bSprinting = false;

	// are we out of recovery mode?
	if (!bRecovering)
	{
		// set the default walk speed
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

		// call the sprint state changed delegate
		OnSprintStateChanged.Broadcast(false);
	}
}

void AHorrorCharacter::SprintFixedTick()
{
	// are we out of recovery, still have stamina and are moving faster than our walk speed?
	if (bSprinting && !bRecovering && GetVelocity().Length() > WalkSpeed)
	{

		// do we still have meter to burn?
		if (SprintMeter > 0.0f)
		{
			// update the sprint meter
			SprintMeter = FMath::Max(SprintMeter - SprintFixedTickTime, 0.0f);

			// have we run out of stamina?
			if (SprintMeter <= 0.0f)
			{
				// raise the recovering flag
				bRecovering = true;

				// set the recovering walk speed
				GetCharacterMovement()->MaxWalkSpeed = RecoveringWalkSpeed;
			}
		}
		
	} else {

		// recover stamina
		SprintMeter = FMath::Min(SprintMeter + SprintFixedTickTime, SprintTime);

		if (SprintMeter >= SprintTime)
		{
			// lower the recovering flag
			bRecovering = false;

			// set the walk or sprint speed depending on whether the sprint button is down
			GetCharacterMovement()->MaxWalkSpeed = bSprinting ? SprintSpeed : WalkSpeed;

			// update the sprint state depending on whether the button is down or not
			OnSprintStateChanged.Broadcast(bSprinting);
		}

	}

	// broadcast the sprint meter updated delegate
	OnSprintMeterUpdated.Broadcast(SprintMeter / SprintTime);

}

void AHorrorCharacter::DoInteract()
{
	if (CurrentInteractable)
	{
		// Try to get the interactable interface
		IInteractable* Interactable = Cast<IInteractable>(CurrentInteractable);
		if (Interactable)
		{
			// Get the player controller
			APlayerController* PC = Cast<APlayerController>(GetController());
			if (PC)
			{
				// Perform the interaction
				Interactable->Execute_OnInteract(CurrentInteractable.Get(), PC);
			}
		}
	}
}

void AHorrorCharacter::DoToggleNotebook()
{
	bIsNotebookOpen = !bIsNotebookOpen;
	OnNotebookToggled.Broadcast(bIsNotebookOpen);

	UE_LOG(LogTemp, Log, TEXT("HorrorCharacter: Notebook %s"), bIsNotebookOpen ? TEXT("opened") : TEXT("closed"));
}

void AHorrorCharacter::CheckForInteractables()
{
	// Don't check if notebook is open
	if (bIsNotebookOpen)
	{
		if (CurrentInteractable)
		{
			SetCurrentInteractable(nullptr);
		}
		return;
	}

	// Get camera location and forward vector
	FVector CameraLocation = GetFirstPersonCameraComponent()->GetComponentLocation();
	FVector CameraForward = GetFirstPersonCameraComponent()->GetForwardVector();

	// Perform line trace
	FVector TraceStart = CameraLocation;
	FVector TraceEnd = CameraLocation + (CameraForward * InteractionDistance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		QueryParams
	);

	// Debug visualization (optional - can be disabled in shipping builds)
	#if !UE_BUILD_SHIPPING
	// DrawDebugLine(GetWorld(), TraceStart, TraceEnd, bHit ? FColor::Green : FColor::Red, false, 0.1f, 0, 1.0f);
	// if (bHit && HitResult.GetActor())
	// {
	// 	DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 8, FColor::Yellow, false, 0.1f);
	// }
	#endif

	if (bHit && HitResult.GetActor())
	{
		// Check if the hit actor implements the interactable interface
		if (HitResult.GetActor()->Implements<UInteractable>())
		{
			IInteractable* Interactable = Cast<IInteractable>(HitResult.GetActor());
			if (Interactable && Interactable->Execute_CanInteract(HitResult.GetActor(), Cast<APlayerController>(GetController())))
			{
				// Set this as the new interactable if it's different
				if (CurrentInteractable != HitResult.GetActor())
				{
					SetCurrentInteractable(HitResult.GetActor());
				}
				return;
			}
		}
	}

	// No valid interactable found
	if (CurrentInteractable)
	{
		SetCurrentInteractable(nullptr);
	}
}

void AHorrorCharacter::SetCurrentInteractable(AActor* NewInteractable)
{
	// Clear previous interactable
	if (CurrentInteractable && CurrentInteractable != NewInteractable)
	{
		IInteractable* OldInteractable = Cast<IInteractable>(CurrentInteractable);
		if (OldInteractable)
		{
			OldInteractable->Execute_OnInteractionFocusLost(CurrentInteractable.Get(), Cast<APlayerController>(GetController()));
		}
		OnInteractableLost.Broadcast();
	}

	// Set new interactable
	CurrentInteractable = NewInteractable;

	if (CurrentInteractable)
	{
		IInteractable* Interactable = Cast<IInteractable>(CurrentInteractable);
		if (Interactable)
		{
			Interactable->Execute_OnInteractionFocus(CurrentInteractable.Get(), Cast<APlayerController>(GetController()));

			FText Prompt = Interactable->Execute_GetInteractionPrompt(CurrentInteractable.Get());
			OnInteractableDetected.Broadcast(CurrentInteractable.Get(), Prompt);
		}
	}
}

