# GrimRailDemo

A technical demonstration project showcasing advanced Unreal Engine 5.6 gameplay systems for narrative-driven horror games.

## 🎥 Video Showcase

<!-- Replace YOUR_VIDEO_ID with your actual YouTube video ID -->
[![GrimRailDemo Showcase](https://img.youtube.com/vi/YOUR_VIDEO_ID/maxresdefault.jpg)](https://www.youtube.com/watch?v=YOUR_VIDEO_ID)

*Click to watch the full demonstration*

---

## 📋 Overview

GrimRailDemo is an Unreal Engine 5.6 first-person game project demonstrating systems designed for psychological horror experiences. Built with a hybrid C++/Blueprint architecture, it showcases key mechanics aligned with modern narrative-driven game design.

### Key Features

**🗒️ Notebook System**
- Component-based quest and narrative tracking
- Categorized entries (Clues, Objectives, Lore, Characters, Environment)
- Real-time UI updates via delegate system
- Read/unread status tracking with visual indicators
- Automatic population from collectible items

**🔄 Room Flip Puzzle Mechanic**
- 180° environmental rotation for spatial puzzles
- Player attachment during rotation with proper physics handling
- Automatic character orientation reset (maintains spatial relationship)
- Smooth interpolated rotation with configurable easing curves
- Event-driven system for gameplay integration

**🎮 Interaction System**
- Raycast-based detection (120cm range, ECC_Visibility channel)
- Interface-driven design (IInteractable)
- Dynamic UI prompt system
- Support for collectibles, triggers, and environmental objects
- Real-time feedback and visual highlighting

**💨 Horror Variant Features**
- Stamina-based sprinting with fixed-tick updates
- Enhanced Input System integration
- Flashlight mechanics (SpotLight component)
- Dual-UI architecture (HUD + overlay widgets)

---

## 🛠️ Technical Stack

**Engine:** Unreal Engine 5.6
**Languages:** C++, Blueprint Visual Scripting
**Build System:** UnrealBuildTool
**Version Control:** Git with Git LFS
**Modules:** Core, CoreUObject, Engine, EnhancedInput, AIModule, StateTreeModule, UMG

### Architecture Highlights

- **Modular Component Design**: NotebookComponent, interaction system as reusable components
- **Interface-Driven**: IInteractable for flexible object interaction
- **Delegate Pattern**: Event-driven UI updates and system communication
- **Abstract Base Classes**: Variant system for different gameplay modes (Horror/Shooter)
- **Enhanced Input System**: Rebindable input actions with mapping contexts

---

## 📂 Project Structure

```
Source/GrimRailDemo/
├── GrimRailDemoCharacter.h/.cpp          # Base character class
├── NotebookComponent.h/.cpp              # Quest tracking system
├── Interactable.h/.cpp                   # Interaction interface
├── CollectibleActor.h/.cpp               # Pickable items (notes, clues)
├── InteractableTrigger.h/.cpp            # Triggers (levers, buttons)
├── RoomFlipActor.h/.cpp                  # Room rotation puzzle
└── Variant_Horror/
    ├── HorrorCharacter.h/.cpp            # Horror variant character
    ├── HorrorPlayerController.h/.cpp     # Widget management, input handling
    └── UI/
        └── HorrorUI.h/.cpp               # Main HUD class

Content/Demo/
├── Blueprints/
│   ├── BP_HorrorCharacter_Demo          # Player character
│   ├── BP_Collectible_Note              # Collectible notes
│   ├── BP_RoomFlip_Compartment          # Rotating room
│   └── BP_RoomFlipTrigger_v2            # Flip trigger
├── UI/
│   ├── WBP_Notebook                     # Main notebook interface
│   ├── WBP_NotebookEntryItem            # Entry list items
│   └── WBP_HorrorUI                     # HUD overlay
└── Maps/
    └── Lvl_GrimRailDemo2                # Demo level
```

### Controls

- **WASD** - Movement
- **Mouse** - Look around
- **Shift** - Sprint
- **E** - Interact with objects
- **Tab** - Toggle notebook

---

## 🎯 Key Systems Demonstrated

### 1. Notebook Component
Fully integrated quest tracking system with:
- `UNotebookComponent` actor component
- Struct-based entry data (`FNotebookEntry`)
- Category filtering and search
- Delegate callbacks for UI updates

**C++ Implementation:**
```cpp
UFUNCTION(BlueprintCallable, Category = "Notebook")
bool AddEntry(const FNotebookEntry& Entry);

UPROPERTY(BlueprintAssignable, Category = "Notebook")
FOnNotebookEntryAdded OnNotebookEntryAdded;
```

### 2. Room Flip Mechanic
Physics-based environmental puzzle:
- Player attachment with `FAttachmentTransformRules::KeepWorldTransform`
- Rotation interpolation with optional curve-based easing
- Character orientation reset (pitch/roll → 0, preserving yaw)
- Input mode management during rotation

**Core Logic:**
```cpp
// Attach player during rotation
AttachedPlayer->AttachToComponent(RoomRoot, FAttachmentTransformRules::KeepWorldTransform);

// Reset player orientation after flip
FRotator UprightRotation = FRotator(0.0f, CurrentRotation.Yaw, 0.0f);
AttachedPlayer->SetActorRotation(UprightRotation);
```

### 3. Interaction System
Flexible interface-based interaction:
- Line trace every 0.1s on `ECC_Visibility` channel
- Interface method delegation (`Execute_OnInteract`, `Execute_CanInteract`)
- Separate focus and interaction events
- Distance-based validation (120cm default)

---

## 📊 Performance Considerations

- **Fixed-tick systems**: Sprint stamina updates at 30Hz (configurable)
- **Interaction checks**: 10Hz raycast updates to reduce CPU overhead
- **Widget optimization**: Visibility-based update gating for UI
- **Component architecture**: Minimal coupling for modular performance profiling

---

## 🔧 Development Notes

### Building New Features

The project uses a **variant system** for different gameplay modes:
- Base classes are marked `UCLASS(abstract)`
- Derived classes implement variant-specific features
- Blueprint child classes provide concrete implementations

**Example workflow:**
1. Extend base C++ classes for new mechanics
2. Create Blueprint child classes for design iteration
3. Use delegates to connect systems without tight coupling

### Best Practices Applied

- Enhanced Input System for rebindable controls
- Interface-driven design for flexible object interaction
- Component-based architecture for reusability
- Delegate pattern for UI and gameplay communication
- Struct-based data for serialization and inspector editing

---

## 🙏 Acknowledgments

- Unreal Engine 5.6 by Epic Games
- First Person Template (TP_FirstPerson) as project foundation
- Claude Code for development assistance

---

*Built with Unreal Engine 5.6 | C++ & Blueprint | Horror Game Systems*
