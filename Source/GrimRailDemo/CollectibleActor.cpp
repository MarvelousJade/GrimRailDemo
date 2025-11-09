// Copyright Epic Games, Inc. All Rights Reserved.

#include "CollectibleActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"

ACollectibleActor::ACollectibleActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create root scene component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Create mesh component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create interaction sphere
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetSphereRadius(150.0f);
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Default values
	bHasBeenCollected = false;
	InteractionPromptText = FText::FromString(TEXT("Pick Up"));
	FocusingPlayerController = nullptr;
}

void ACollectibleActor::BeginPlay()
{
	Super::BeginPlay();

	// Store initial Z position for floating animation
	InitialZPosition = GetActorLocation().Z;

	// Validate notebook entry
	if (NotebookEntry.EntryID.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("CollectibleActor '%s' has no EntryID set!"), *GetName());
	}
}

void ACollectibleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bHasBeenCollected)
	{
		UpdateVisualEffects(DeltaTime);
	}
}

void ACollectibleActor::OnInteractionFocus_Implementation(APlayerController* PlayerController)
{
	FocusingPlayerController = PlayerController;

	// Call Blueprint event
	BP_OnFocusGained(PlayerController);

	UE_LOG(LogTemp, Verbose, TEXT("CollectibleActor '%s' gained focus"), *GetName());
}

void ACollectibleActor::OnInteractionFocusLost_Implementation(APlayerController* PlayerController)
{
	FocusingPlayerController = nullptr;

	// Call Blueprint event
	BP_OnFocusLost(PlayerController);

	UE_LOG(LogTemp, Verbose, TEXT("CollectibleActor '%s' lost focus"), *GetName());
}

bool ACollectibleActor::OnInteract_Implementation(APlayerController* PlayerController)
{
	if (!CanInteract_Implementation(PlayerController))
	{
		return false;
	}

	PerformCollection(PlayerController);
	return true;
}

FText ACollectibleActor::GetInteractionPrompt_Implementation() const
{
	return InteractionPromptText;
}

bool ACollectibleActor::CanInteract_Implementation(APlayerController* PlayerController) const
{
	// Can't interact if already collected (unless multiple collections allowed)
	if (bHasBeenCollected && !bCanBeCollectedMultipleTimes)
	{
		return false;
	}

	// Must have a valid player controller
	if (!PlayerController)
	{
		return false;
	}

	return true;
}

void ACollectibleActor::PerformCollection(APlayerController* Collector)
{
	if (!Collector)
	{
		return;
	}

	// Get the pawn's notebook component
	APawn* Pawn = Collector->GetPawn();
	if (!Pawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("CollectibleActor: Player pawn is null"));
		return;
	}

	UNotebookComponent* NotebookComponent = Pawn->FindComponentByClass<UNotebookComponent>();
	if (!NotebookComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("CollectibleActor: Player pawn has no NotebookComponent"));
		return;
	}

	// Validate entry data before adding
	if (NotebookEntry.EntryID.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("CollectibleActor '%s': NotebookEntry has no EntryID! Cannot collect."), *GetName());
		return;
	}

	// Add the entry to the notebook
	if (NotebookComponent->AddEntry(NotebookEntry))
	{
		// Mark as collected
		bHasBeenCollected = true;

		// Call Blueprint event
		BP_OnCollected(Collector);

		UE_LOG(LogTemp, Log, TEXT("CollectibleActor '%s' collected by player"), *GetName());

		// Destroy this actor if configured to do so
		if (bDestroyOnCollect)
		{
			// Hide immediately
			MeshComponent->SetVisibility(false);
			SetActorEnableCollision(false);

			// Destroy after delay (allows for effects/sounds to play)
			FTimerHandle DestroyTimerHandle;
			GetWorldTimerManager().SetTimer(DestroyTimerHandle, [this]()
			{
				Destroy();
			}, DestroyDelay, false);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CollectibleActor: Failed to add entry to notebook (may already exist)"));
	}
}

void ACollectibleActor::UpdateVisualEffects(float DeltaTime)
{
	FVector NewLocation = GetActorLocation();
	FRotator NewRotation = GetActorRotation();

	// Floating animation
	if (bEnableFloating)
	{
		const float Time = GetWorld()->GetTimeSeconds();
		const float FloatOffset = FMath::Sin(Time * FloatingSpeed) * FloatingAmplitude;
		NewLocation.Z = InitialZPosition + FloatOffset;
	}

	// Rotation animation
	if (bEnableRotation)
	{
		NewRotation.Yaw += RotationSpeed * DeltaTime;
	}

	SetActorLocationAndRotation(NewLocation, NewRotation);
}
