# GrimRail Demo - Portfolio Guide

This demo showcases the core systems requested in the Infinite Loop Studios job posting:
1. **Notebook System** - Quest tracking and narrative progression
2. **Room Flip Mechanic** - Spatial puzzle system
3. **Interaction System** - Collectibles and environmental storytelling

## What We've Built (C++ Core Systems)

### 1. Notebook System
**Files**: `NotebookComponent.h/.cpp`

**Features**:
- Categorized entries (Clues, Objectives, Lore, Characters, Environment)
- Entry metadata (title, body, timestamp, read status, images)
- Delegates for UI updates when entries are added/read
- Blueprint-accessible API for easy integration

**Key Functions**:
- `AddEntry()` - Add new notebook entries
- `GetEntriesByCategory()` - Filter entries by type
- `MarkEntryAsRead()` - Track which entries player has viewed
- `GetUnreadCount()` - Show notification badges

### 2. Interaction System
**Files**: `Interactable.h/.cpp`, `CollectibleActor.h/.cpp`

**Features**:
- Interface-based design (any actor can be interactable)
- Raycast detection from camera
- Focus/unfocus events for visual feedback
- Interaction prompts system
- Floating/rotating collectible animations

**Key Components**:
- `IInteractable` - Interface for all interactable objects
- `ACollectibleActor` - Base class for pickups that add notebook entries
- Automatic visual effects (bob/rotate)
- Blueprint events for custom behavior

### 3. Room Flip Mechanic
**Files**: `RoomFlipActor.h/.cpp`

**Features**:
- Smooth interpolated rotation around any axis
- Player attachment during rotation (maintains spatial relationship)
- Optional input disabling during flip
- Customizable rotation angle, duration, easing
- Support for multiple flips (toggle mode)
- Blueprint events at key moments (start, progress, complete)

**Key Functions**:
- `TriggerFlip()` - Start the room rotation
- `CanFlip()` - Check if flip is allowed
- `ResetRoom()` - Return to original state

### 4. Enhanced Horror Character
**Files**: `HorrorCharacter.h/.cpp` (modified)

**New Features**:
- Integrated NotebookComponent
- Continuous interaction checking (raycasts every 0.1s)
- Notebook toggle input
- Interaction input
- Delegates for UI updates (interaction prompts, notebook state)

## Next Steps to Complete the Demo

### STEP 1: Build the Project ⚙️

1. **Open the project in Unreal Editor 5.6**
   - Right-click `GrimRailDemo.uproject` → Generate Visual Studio project files
   - Open `GrimRailDemo.sln` in Visual Studio

2. **Build the project**
   - Set configuration to `Development Editor`
   - Build → Build Solution (or Ctrl+Shift+B)
   - Fix any compilation errors (there shouldn't be any!)

3. **Launch Unreal Editor**
   - Once compiled, open `GrimRailDemo.uproject`
   - All new C++ classes should appear in Content Browser

### STEP 2: Create Blueprint Classes 🎨

Create Blueprint children of the C++ classes:

1. **BP_HorrorCharacter_Demo**
   - Parent: `AHorrorCharacter`
   - Set up Input Actions:
     - Create `IA_Interact` (Action key: E)
     - Create `IA_ToggleNotebook` (Action key: Tab)
     - Assign to InteractAction and ToggleNotebookAction properties
   - Add to Input Mapping Context (IMC_Horror)

2. **BP_Collectible_Note**
   - Parent: `ACollectibleActor`
   - Set NotebookEntry values:
     - EntryID: "first_note"
     - Title: "Passenger's Note"
     - Body: "The train never stops... I've been here for days. Or has it been years?"
     - Category: Clue
   - Set mesh (use simple cube or paper mesh from starter content)
   - Optionally implement BP_OnCollected for sound/particles

3. **BP_Collectible_Clue**
   - Similar to note but different category
   - Example: Character info, lore, etc.

4. **BP_RoomFlip_Compartment**
   - Parent: `ARoomFlipActor`
   - Configure:
     - RotationAxis: X_Axis (for roll) or Y_Axis (for pitch flip)
     - RotationAngle: 180.0
     - RotationDuration: 3.0 seconds
     - bAttachPlayer: true
     - bDisablePlayerInput: true
   - Build compartment geometry as children of RoomRoot component
   - Add collectibles as children so they flip with room

### STEP 3: Create Notebook UI 📓

1. **Create Widget Blueprint: WBP_NotebookUI**

   **Layout**:
   ```
   Canvas
   ├─ Background (Dark overlay, fills screen)
   ├─ Notebook Panel (Center, 60% screen width)
   │  ├─ Tab Bar (Categories)
   │  │  ├─ Button: Clues
   │  │  ├─ Button: Objectives
   │  │  ├─ Button: Lore
   │  │  └─ Button: Characters
   │  ├─ Entry List (Scroll box)
   │  │  └─ Entry buttons (generated from notebook)
   │  └─ Entry Detail Panel
   │     ├─ Title (Text)
   │     ├─ Body (Rich text)
   │     └─ Image (optional)
   └─ Close Hint ("Press Tab to close")
   ```

   **Blueprint Logic**:
   - On Construct: Get NotebookComponent from player character
   - Bind to OnNotebookEntryAdded delegate
   - Function: RefreshEntryList() - Populates scroll box from GetEntriesByCategory()
   - Function: DisplayEntry(EntryID) - Shows details, calls MarkEntryAsRead()

2. **Create Widget Blueprint: WBP_InteractionPrompt**

   **Layout**:
   ```
   Canvas
   └─ Center Bottom
      └─ Text: "[E] Pick Up Note"
   ```

   **Blueprint Logic**:
   - Bind visibility to HorrorCharacter→OnInteractableDetected/Lost
   - Update text from delegate parameter

3. **Integrate with HorrorPlayerController**
   - On BeginPlay: Create WBP_InteractionPrompt, add to viewport
   - Bind to Character→OnNotebookToggled:
     - If opened: Create WBP_NotebookUI, add to viewport, show mouse cursor
     - If closed: Remove widget, hide mouse cursor

### STEP 4: Build the Test Level 🏗️

Create a new level: **Lvl_GrimRailDemo**

**Scene Layout**:

```
Train Compartment (single car, 15m x 3m x 3m)
├─ Floor/Walls/Ceiling (Static meshes or BSP)
├─ BP_RoomFlip_Compartment (contains room geometry)
│  ├─ Seats (attached to RoomRoot)
│  ├─ Windows (one side shows "normal" view, flip reveals "mirror" view)
│  ├─ BP_Collectible_Note_01 (on seat)
│  ├─ BP_Collectible_Clue_01 (hidden, only visible after flip)
│  └─ BP_Collectible_Clue_02 (under flipped seat)
├─ PlayerStart
└─ Lighting (dim horror atmosphere)
```

**Puzzle Flow**:
1. Player spawns in compartment
2. Finds note explaining the room flip mechanic
3. Collects 2-3 clues to "unlock" the flip (optional trigger logic)
4. Room flips 180°, revealing new collectibles/passage
5. Player finds final clue explaining what happened

**Trigger the Flip**:
- Option A: Collect all clues → auto-trigger flip
- Option B: Press E on a "lever" actor that calls RoomFlipActor→TriggerFlip()
- Option C: Blueprint sequence after timer

### STEP 5: Polish 💎

**Atmosphere**:
- Dark lighting (horror game)
- Use HorrorCharacter's spotlight
- Add simple sound: ambient train rumble, wind

**Materials**:
- Use M_FillBackground for notebook UI
- Simple materials on collectibles (emissive glow when focused)

**Camera Effects**:
- Add slight camera shake during room flip
- Post-process volume (desaturation, vignette)

**Audio Hooks** (implement in Blueprint):
- Collectible pickup sound (BP_OnCollected event)
- Notebook open/close sound (OnNotebookToggled)
- Room flip rumble (BP_OnFlipStarted)

## Showcasing Your Work

### What to Highlight in Your Application:

**C++ Systems** (Primary Focus):
1. **Architecture**: Clean, modular, interface-driven design
2. **Component-based**: NotebookComponent can be added to any pawn
3. **Blueprint Integration**: Lots of BlueprintCallable/BlueprintPure functions
4. **Delegates**: Proper event-driven architecture for UI updates
5. **Polish**: Easing curves, smooth interpolation, configurable parameters

**Key Code to Point Out**:
- `NotebookComponent.h:41-55` - FNotebookEntry struct design
- `Interactable.h:23-55` - Interface pattern for extensibility
- `RoomFlipActor.cpp:105-140` - Smooth rotation with easing
- `HorrorCharacter.cpp:195-255` - Raycast interaction system

### Video/Screenshot Checklist:

✅ Notebook UI showing categorized entries
✅ Picking up collectible → entry appears in notebook
✅ Room flip in action (player rotates with room)
✅ Before/after flip showing hidden collectibles
✅ Interaction prompt appearing when looking at objects
✅ Code screenshots showing C++ implementation

### Presentation Tips:

**In your email/portfolio**:
```
Subject: Unreal Engine Programmer Application - [Your Name]

Hi Infinite Loop Studios team,

I've created a demo specifically showcasing the systems mentioned in your job posting:

📓 Notebook System - Full quest/narrative tracking with categorized entries
🔄 Room Flip Mechanic - Spatial puzzle with smooth 180° rotation
🎮 Interaction System - Raycast-based collectibles integrated with notebook

Tech Stack:
- Unreal Engine 5.6, C++17
- Component-based architecture
- Interface-driven design for extensibility
- Enhanced Input system integration
- Blueprint-exposed API for designers

Demo Link: [Playable build or video]
Source Code: [GitHub repo if sharing]

The implementation demonstrates my ability to build gameplay systems from scratch,
integrate with existing codebases, and design for both programmer and designer workflows.

Looking forward to discussing how I can contribute to GrimRail!

Best,
[Your Name]
```

## Quick Reference: Build & Run

```bash
# Generate project files
Right-click GrimRailDemo.uproject → Generate Visual Studio project files

# Build in Visual Studio
Configuration: Development Editor
Platform: Win64
Build Solution (Ctrl+Shift+B)

# Or build via command line
"C:\Program Files\Epic Games\UE_5.6\Engine\Build\BatchFiles\Build.bat" GrimRailDemoEditor Win64 Development "G:\Unreal Projects\GrimRailDemo\GrimRailDemo.uproject" -waitmutex

# Run in editor
Open GrimRailDemo.uproject
PIE (Alt+P)
```

## Timeline Estimate

- ✅ C++ Core Systems: **COMPLETE**
- ⏱️ Blueprint Setup: **2-3 hours**
- ⏱️ UI Creation: **3-4 hours**
- ⏱️ Test Level: **2-3 hours**
- ⏱️ Polish & Recording: **2-3 hours**

**Total**: ~10-13 hours to fully polished demo

## Troubleshooting

**Compilation Errors**:
- Make sure UE 5.6 is installed
- Check that all #include paths are correct
- Rebuild solution from clean state

**Notebook not showing entries**:
- Check NotebookComponent is created in HorrorCharacter constructor
- Verify BlueprintAssignable delegates are bound in widget
- Check EntryID is not NAME_None

**Room flip not working**:
- Ensure player is within room bounds when flip triggers
- Check RoomRoot has child geometry attached
- Verify RotationDuration > 0

**Interaction not detecting**:
- Check InteractionDistance (default 200cm)
- Verify collectible has collision enabled
- Ensure InteractAction input is bound

Good luck with your application! This demo directly addresses everything they asked for.
