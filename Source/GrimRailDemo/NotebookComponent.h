// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NotebookComponent.generated.h"

/** Categories for organizing notebook entries */
UENUM(BlueprintType)
enum class ENotebookCategory : uint8
{
	Clue			UMETA(DisplayName = "Clues"),
	Objective		UMETA(DisplayName = "Objectives"),
	Lore			UMETA(DisplayName = "Lore"),
	Character		UMETA(DisplayName = "Characters"),
	Environment		UMETA(DisplayName = "Environment")
};

/** A single entry in the player's notebook */
USTRUCT(BlueprintType)
struct FNotebookEntry
{
	GENERATED_BODY()

	/** Unique identifier for this entry */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notebook")
	FName EntryID;

	/** Display title of the entry */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notebook")
	FText Title;

	/** Full body text of the entry */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notebook")
	FText Body;

	/** Category this entry belongs to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notebook")
	ENotebookCategory Category = ENotebookCategory::Clue;

	/** Game time when this entry was added */
	UPROPERTY(BlueprintReadOnly, Category = "Notebook")
	float Timestamp = 0.0f;

	/** Whether the player has read this entry */
	UPROPERTY(BlueprintReadWrite, Category = "Notebook")
	bool bHasBeenRead = false;

	/** Optional image/icon for this entry */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notebook")
	TObjectPtr<UTexture2D> EntryImage;

	FNotebookEntry()
		: EntryID(NAME_None)
		, Category(ENotebookCategory::Clue)
		, Timestamp(0.0f)
		, bHasBeenRead(false)
		, EntryImage(nullptr)
	{}
};

/** Delegate called when a new entry is added to the notebook */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNotebookEntryAdded, const FNotebookEntry&, Entry, int32, UnreadCount);

/** Delegate called when an entry is marked as read */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotebookEntryRead, const FNotebookEntry&, Entry);

/**
 * Component that manages the player's notebook system
 * Tracks clues, objectives, lore, and character information discovered during gameplay
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GRIMRAILDEMO_API UNotebookComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	/** All notebook entries collected by the player */
	UPROPERTY(BlueprintReadOnly, Category = "Notebook")
	TArray<FNotebookEntry> Entries;

	/** Cached count of unread entries for UI updates */
	int32 UnreadCount = 0;

public:

	/** Delegate broadcast when a new entry is added */
	UPROPERTY(BlueprintAssignable, Category = "Notebook")
	FOnNotebookEntryAdded OnNotebookEntryAdded;

	/** Delegate broadcast when an entry is read */
	UPROPERTY(BlueprintAssignable, Category = "Notebook")
	FOnNotebookEntryRead OnNotebookEntryRead;

public:

	UNotebookComponent();

protected:

	virtual void BeginPlay() override;

public:

	/**
	 * Adds a new entry to the notebook
	 * @param Entry The notebook entry to add
	 * @return True if the entry was added, false if it already exists
	 */
	UFUNCTION(BlueprintCallable, Category = "Notebook")
	bool AddEntry(const FNotebookEntry& Entry);

	/**
	 * Marks an entry as read
	 * @param EntryID The unique ID of the entry to mark as read
	 */
	UFUNCTION(BlueprintCallable, Category = "Notebook")
	void MarkEntryAsRead(FName EntryID);

	/**
	 * Gets all entries in the notebook
	 * @return Array of all notebook entries
	 */
	UFUNCTION(BlueprintPure, Category = "Notebook")
	const TArray<FNotebookEntry>& GetAllEntries() const { return Entries; }

	/**
	 * Gets all entries in a specific category
	 * @param Category The category to filter by
	 * @return Array of entries in the specified category
	 */
	UFUNCTION(BlueprintPure, Category = "Notebook")
	TArray<FNotebookEntry> GetEntriesByCategory(ENotebookCategory Category) const;

	/**
	 * Gets a specific entry by its ID
	 * @param EntryID The unique ID of the entry
	 * @param OutEntry The found entry (output parameter)
	 * @return True if the entry was found
	 */
	UFUNCTION(BlueprintPure, Category = "Notebook")
	bool GetEntryByID(FName EntryID, FNotebookEntry& OutEntry) const;

	/**
	 * Checks if an entry exists in the notebook
	 * @param EntryID The unique ID to check
	 * @return True if the entry exists
	 */
	UFUNCTION(BlueprintPure, Category = "Notebook")
	bool HasEntry(FName EntryID) const;

	/**
	 * Gets the number of unread entries
	 * @return Count of unread entries
	 */
	UFUNCTION(BlueprintPure, Category = "Notebook")
	int32 GetUnreadCount() const { return UnreadCount; }

	/**
	 * Gets the total number of entries
	 * @return Total entry count
	 */
	UFUNCTION(BlueprintPure, Category = "Notebook")
	int32 GetTotalEntryCount() const { return Entries.Num(); }

	/**
	 * Clears all notebook entries (for testing/debug)
	 */
	UFUNCTION(BlueprintCallable, Category = "Notebook")
	void ClearAllEntries();

protected:

	/** Updates the unread entry count */
	void UpdateUnreadCount();
};
