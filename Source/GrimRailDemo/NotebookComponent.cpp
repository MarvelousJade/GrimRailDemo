// Copyright Epic Games, Inc. All Rights Reserved.

#include "NotebookComponent.h"
#include "GameFramework/PlayerController.h"

UNotebookComponent::UNotebookComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	UnreadCount = 0;
}

void UNotebookComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UNotebookComponent::AddEntry(const FNotebookEntry& Entry)
{
	// Check if entry already exists
	if (HasEntry(Entry.EntryID))
	{
		UE_LOG(LogTemp, Warning, TEXT("NotebookComponent: Entry '%s' already exists, skipping add"), *Entry.EntryID.ToString());
		return false;
	}

	// Create a new entry with current timestamp
	FNotebookEntry NewEntry = Entry;
	NewEntry.Timestamp = GetWorld()->GetTimeSeconds();
	NewEntry.bHasBeenRead = false;

	// Add to entries array
	Entries.Add(NewEntry);

	// Update unread count
	UnreadCount++;

	// Broadcast delegate
	OnNotebookEntryAdded.Broadcast(NewEntry, UnreadCount);

	UE_LOG(LogTemp, Log, TEXT("NotebookComponent: Added entry '%s' - %s"), *Entry.EntryID.ToString(), *Entry.Title.ToString());

	return true;
}

void UNotebookComponent::MarkEntryAsRead(FName EntryID)
{
	for (FNotebookEntry& Entry : Entries)
	{
		if (Entry.EntryID == EntryID && !Entry.bHasBeenRead)
		{
			Entry.bHasBeenRead = true;
			UpdateUnreadCount();
			OnNotebookEntryRead.Broadcast(Entry);

			UE_LOG(LogTemp, Log, TEXT("NotebookComponent: Marked entry '%s' as read"), *EntryID.ToString());
			return;
		}
	}
}

TArray<FNotebookEntry> UNotebookComponent::GetEntriesByCategory(ENotebookCategory Category) const
{
	TArray<FNotebookEntry> FilteredEntries;

	for (const FNotebookEntry& Entry : Entries)
	{
		if (Entry.Category == Category)
		{
			FilteredEntries.Add(Entry);
		}
	}

	return FilteredEntries;
}

bool UNotebookComponent::GetEntryByID(FName EntryID, FNotebookEntry& OutEntry) const
{
	for (const FNotebookEntry& Entry : Entries)
	{
		if (Entry.EntryID == EntryID)
		{
			OutEntry = Entry;
			return true;
		}
	}

	return false;
}

bool UNotebookComponent::HasEntry(FName EntryID) const
{
	for (const FNotebookEntry& Entry : Entries)
	{
		if (Entry.EntryID == EntryID)
		{
			return true;
		}
	}

	return false;
}

void UNotebookComponent::ClearAllEntries()
{
	Entries.Empty();
	UnreadCount = 0;
	UE_LOG(LogTemp, Log, TEXT("NotebookComponent: Cleared all entries"));
}

void UNotebookComponent::UpdateUnreadCount()
{
	UnreadCount = 0;
	for (const FNotebookEntry& Entry : Entries)
	{
		if (!Entry.bHasBeenRead)
		{
			UnreadCount++;
		}
	}
}
