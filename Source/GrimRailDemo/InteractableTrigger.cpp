// Copyright Epic Games, Inc. All Rights Reserved.

#include "InteractableTrigger.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

AInteractableTrigger::AInteractableTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create root scene component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Create trigger mesh
	TriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriggerMesh"));
	TriggerMesh->SetupAttachment(RootComponent);
	TriggerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create interaction box
	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(RootComponent);
	InteractionBox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	InteractionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// Default values
	InteractionPromptText = FText::FromString(TEXT("Press E to Interact"));
	bCanInteract = true;
	bSingleUse = false;
	bHasBeenUsed = false;
}

void AInteractableTrigger::BeginPlay()
{
	Super::BeginPlay();
}

void AInteractableTrigger::OnInteractionFocus_Implementation(APlayerController* PlayerController)
{
	// Call Blueprint event
	BP_OnFocusGained(PlayerController);

	UE_LOG(LogTemp, Verbose, TEXT("InteractableTrigger '%s' gained focus"), *GetName());
}

void AInteractableTrigger::OnInteractionFocusLost_Implementation(APlayerController* PlayerController)
{
	// Call Blueprint event
	BP_OnFocusLost(PlayerController);

	UE_LOG(LogTemp, Verbose, TEXT("InteractableTrigger '%s' lost focus"), *GetName());
}

bool AInteractableTrigger::OnInteract_Implementation(APlayerController* PlayerController)
{
	if (!CanInteract_Implementation(PlayerController))
	{
		return false;
	}

	// Mark as used if single-use
	if (bSingleUse)
	{
		bHasBeenUsed = true;
	}

	// Call Blueprint event
	BP_OnTriggered(PlayerController);

	UE_LOG(LogTemp, Log, TEXT("InteractableTrigger '%s' triggered by player"), *GetName());

	return true;
}

FText AInteractableTrigger::GetInteractionPrompt_Implementation() const
{
	return InteractionPromptText;
}

bool AInteractableTrigger::CanInteract_Implementation(APlayerController* PlayerController) const
{
	// Can't interact if disabled
	if (!bCanInteract)
	{
		return false;
	}

	// Can't interact if already used and single-use
	if (bSingleUse && bHasBeenUsed)
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
