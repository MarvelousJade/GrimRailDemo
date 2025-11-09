# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

GrimRailDemo is an Unreal Engine 5.6 first-person game project demonstrating two gameplay variants built on a shared base character system:
- **Horror variant**: Stamina-based sprinting with flashlight mechanics
- **Shooter variant**: Weapon-based combat with AI enemies using StateTree

## Build System

**Building the Project:**

1. **From Unreal Editor:**
   - Use **Live Coding** (Ctrl+Alt+F11) to compile C++ changes while editor is running
   - Fastest iteration for small changes

2. **From Visual Studio:**
   - Open `GrimRailDemo.sln`
   - Build solution: Development Editor (x64) configuration
   - Launch from editor or VS debugger (F5)

3. **From Command Line:**
   ```
   "UE_5.6\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" GrimRailDemoEditor Win64 Development -Project="GrimRailDemo.uproject"
   ```

4. **Adding New C++ Files:**
   - Create files manually in `Source/GrimRailDemo/` folder structure
   - Or use Tools → New C++ Class in Unreal Editor (recommended)
   - After adding files: Right-click `.uproject` → Generate Visual Studio project files
   - Rebuild solution

**Module Structure:**
- Single runtime module: `GrimRailDemo`
- Build configuration: `Source/GrimRailDemo/GrimRailDemo.Build.cs`
- Target files: `GrimRailDemo.Target.cs` (Game), `GrimRailDemoEditor.Target.cs` (Editor)
- Engine version: UE 5.6 (BuildSettingsVersion.V5, EngineIncludeOrderVersion.Unreal5_6)

**Key Dependencies:**
- Core, CoreUObject, Engine, InputCore, EnhancedInput
- AIModule (for shooter AI)
- StateTreeModule, GameplayStateTreeModule (for AI behavior)
- UMG, Slate (for UI)

**Enabled Plugins:**
- ModelingToolsEditorMode (Editor only)
- StateTree
- GameplayStateTree

**Project Configuration:**
- Target platform: Windows (DirectX 12, Shader Model 6)
- Based on First Person Template (TP_FirstPerson)
- Rendering: Ray tracing, Lumen, Nanite enabled
- Custom collision channel: Projectile
- Default game mode: BP_HorrorGameMode

## Architecture

### Class Hierarchy

**Base Character System:**
- `AGrimRailDemoCharacter` (abstract): Base first-person character with Enhanced Input support
  - First-person mesh (arms) and camera
  - Input actions: Move, Look, MouseLook, Jump
  - Virtual methods for input handling: `DoMove()`, `DoAim()`, `DoJumpStart()`, `DoJumpEnd()`

**Horror Variant:**
- `AHorrorCharacter` → extends `AGrimRailDemoCharacter`
  - Adds stamina-based sprinting system with fixed-tick updates
  - SpotLight component for flashlight
  - **NotebookComponent** for quest tracking and narrative progression
  - **Interaction system** with raycast detection (checks every 0.1s)
    - InteractionDistance: 120cm (default) - designed for arm's reach interaction
    - Uses ECC_Visibility channel for raycasts
    - Detects actors implementing IInteractable interface
  - Input actions: Sprint, Interact (E), ToggleNotebook (Tab)
  - Delegates: `FUpdateSprintMeterDelegate`, `FSprintStateChangedDelegate`, `FOnInteractableDetected`, `FOnInteractableLost`, `FOnNotebookToggled`
  - States: sprinting, recovering, walking

**Shooter Variant:**
- `AShooterCharacter` → extends `AGrimRailDemoCharacter`, implements `IShooterWeaponHolder`
  - Weapon inventory management
  - Health system (CurrentHP/MaxHP)
  - Team identification (TeamByte)
  - PawnNoiseEmitter for AI perception
  - Delegates: `FBulletCountUpdatedDelegate`, `FDamagedDelegate`
  - Input actions: Fire, SwitchWeapon

- `AShooterNPC` → extends `AGrimRailDemoCharacter`, implements `IShooterWeaponHolder`
  - AI-controlled enemy character
  - Single weapon holder
  - StateTree-driven behavior (via `AShooterAIController`)
  - Ragdoll death with deferred destruction
  - Methods: `StartShooting()`, `StopShooting()`
  - Delegate: `FPawnDeathDelegate`

### Weapon System

**Interface-based design:**
- `IShooterWeaponHolder` interface defines contract between characters and weapons
  - `AttachWeaponMeshes()`, `PlayFiringMontage()`, `AddWeaponRecoil()`
  - `GetWeaponTargetLocation()`, `UpdateWeaponHUD()`
  - `AddWeaponClass()`, `OnWeaponActivated()`, `OnWeaponDeactivated()`
  - `OnSemiWeaponRefire()` (notifies holder that semi-auto cooldown expired)

**Weapon classes:**
- `AShooterWeapon` (abstract): Base weapon actor
  - Dual mesh system: FirstPersonMesh and ThirdPersonMesh
  - Ammo system with magazine size and current bullets
  - Firing modes: semi-auto (refire cooldown) or full-auto (timer-based)
  - AnimInstance swapping for different weapon types
  - Aim variance and recoil
  - AI perception integration (shot noise with loudness/range/tag)
  - Projectile-based firing system

- `AShooterProjectile`: Projectile base class spawned by weapons
- `AShooterPickup`: Weapon pickup base class
- `AShooterWeaponHolder`: Component for managing weapon attachment

**Weapon types (Blueprints):**
- BP_ShooterWeapon_Pistol
- BP_ShooterWeapon_Rifle
- BP_ShooterWeapon_GrenadeLauncher

### AI System

**Controller:**
- `AShooterAIController` → extends `AAIController`
  - StateTreeAIComponent for behavior execution
  - AIPerceptionComponent for sight/hearing
  - Team-based friend/foe identification via TeamTag
  - Target tracking (TargetEnemy)
  - Delegates: `FShooterPerceptionUpdatedDelegate`, `FShooterPerceptionForgottenDelegate`
  - Responds to pawn death to clean up

**Behavior:**
- StateTree assets drive AI behavior (ST_Shooter, ST_Shooter_ShootAtTarget)
- Environment Query System (EQS) for positioning:
  - EQS_FindRoamLocation
  - EQS_FindSnipingLocation
- `EnvQueryContext_Target` for custom EQS context
- `ShooterStateTreeUtility` provides helper functions for StateTree tasks

### Game Modes

**Hierarchy:**
- `AGrimRailDemoGameMode` (abstract base)
  - `AHorrorGameMode`
  - `AShooterGameMode`

**Player Controllers:**
- `AGrimRailDemoPlayerController` (base)
  - `AHorrorPlayerController`
  - `AShooterPlayerController`

**Camera:**
- `AGrimRailDemoCameraManager`: Custom camera manager for specialized effects

### Notebook & Interaction Systems (NEW - Added for GrimRail Demo)

**Notebook System:**
- `UNotebookComponent`: Actor component for tracking player discoveries
  - Categorized entries: Clues, Objectives, Lore, Characters, Environment
  - Entry metadata: title, body, category, timestamp, read status, optional images
  - Functions: `AddEntry()`, `GetEntriesByCategory()`, `MarkEntryAsRead()`, `GetUnreadCount()`
  - Delegates: `OnNotebookEntryAdded`, `OnNotebookEntryRead`

**Interaction System:**
- `IInteractable`: Interface for interactable objects (BlueprintNativeEvent functions)
  - Functions: `OnInteractionFocus()`, `OnInteractionFocusLost()`, `OnInteract()`, `GetInteractionPrompt()`, `CanInteract()`
  - **IMPORTANT**: When calling interface functions from C++, use `Execute_FunctionName()` or call `FunctionName_Implementation()` within the same class
  - **IMPORTANT**: BlueprintNativeEvent interface functions MUST be called via `Execute_` methods when calling from outside, or `_Implementation` suffix when calling within the implementing class
- `ACollectibleActor`: Pickups that add notebook entries when collected
  - Automatic visual effects: floating animation, rotation
  - Blueprint events: `BP_OnCollected`, `BP_OnFocusGained`, `BP_OnFocusLost`
  - Configurable: destroy on collect, collect multiple times, interaction prompt text
  - Validates NotebookEntry.EntryID before adding to prevent crashes
- `AInteractableTrigger`: Base class for simple interactable triggers (levers, buttons, switches)
  - Pre-configured collision: InteractionBox blocks Visibility channel
  - Blueprint events: `BP_OnTriggered`, `BP_OnFocusGained`, `BP_OnFocusLost`
  - Configurable: interaction prompt, single-use mode, can interact state
  - Designed for Blueprint subclasses - provides clean interface without exposing C++ implementation

**Room Flip Mechanic:**
- `ARoomFlipActor`: Rotatable room/compartment for spatial puzzles
  - Smooth interpolated rotation around X/Y/Z axis
  - Player attachment during rotation (maintains spatial relationship)
  - Configurable: rotation angle (default 180°), duration, easing curve
  - Optional input disabling during flip
  - Toggle mode for multiple flips
  - Functions: `TriggerFlip()`, `CanFlip()`, `ResetRoom()`
  - Delegates: `OnRoomFlipStarted`, `OnRoomFlipCompleted`, `OnRoomFlipProgress`
  - Blueprint events: `BP_OnFlipStarted`, `BP_OnFlipCompleted`, `BP_OnFlipProgress`

### UI System

**Horror UI:**
- `UHorrorUI`: UMG widget for horror variant
  - Displays sprint meter

**Shooter UI:**
- `UShooterUI`: Main UMG widget for shooter variant
- `UShooterBulletCounterUI`: Ammo counter widget

## Code Organization

```
Source/GrimRailDemo/
├── GrimRailDemo.h/.cpp              # Module definition
├── GrimRailDemoCharacter.h/.cpp     # Base character
├── GrimRailDemoGameMode.h/.cpp      # Base game mode
├── GrimRailDemoPlayerController.h/.cpp
├── GrimRailDemoCameraManager.h/.cpp
├── NotebookComponent.h/.cpp         # Notebook system
├── Interactable.h/.cpp              # Interaction interface
├── CollectibleActor.h/.cpp          # Collectible pickups (notes, items)
├── InteractableTrigger.h/.cpp       # Base class for triggers (levers, buttons)
├── RoomFlipActor.h/.cpp             # Room rotation puzzle mechanic
├── Variant_Horror/
│   ├── HorrorCharacter.h/.cpp       # MODIFIED: Added notebook + interaction (120cm range)
│   ├── HorrorGameMode.h/.cpp
│   ├── HorrorPlayerController.h/.cpp
│   └── UI/
│       └── HorrorUI.h/.cpp
└── Variant_Shooter/
    ├── ShooterCharacter.h/.cpp
    ├── ShooterGameMode.h/.cpp
    ├── ShooterPlayerController.h/.cpp
    ├── AI/
    │   ├── ShooterAIController.h/.cpp
    │   ├── ShooterNPC.h/.cpp
    │   ├── ShooterStateTreeUtility.h/.cpp
    │   └── EnvQueryContext_Target.h/.cpp
    ├── Weapons/
    │   ├── ShooterWeaponHolder.h       # Interface
    │   ├── ShooterWeapon.h/.cpp
    │   ├── ShooterProjectile.h/.cpp
    │   └── ShooterPickup.h/.cpp
    └── UI/
        ├── ShooterUI.h/.cpp
        └── ShooterBulletCounterUI.h/.cpp
```

**Include paths configured in Build.cs:**
- `GrimRailDemo`
- `GrimRailDemo/Variant_Horror`
- `GrimRailDemo/Variant_Horror/UI`
- `GrimRailDemo/Variant_Shooter`
- `GrimRailDemo/Variant_Shooter/AI`
- `GrimRailDemo/Variant_Shooter/UI`
- `GrimRailDemo/Variant_Shooter/Weapons`

## Key Design Patterns

**Variant System:**
- Base classes marked `abstract` (UCLASS(abstract))
- Shared functionality in base `GrimRailDemo*` classes
- Variant-specific features in `Horror*` and `Shooter*` derived classes
- Blueprint child classes implement concrete instances

**Interface-Driven Weapons:**
- `IShooterWeaponHolder` decouples weapon logic from character implementation
- Both player (`AShooterCharacter`) and AI (`AShooterNPC`) implement same interface
- Weapons communicate with holders only through interface methods

**Delegate-Based UI Updates:**
- Characters expose delegates for UI events (sprint meter, bullet count, damage, notebook state, interaction prompts)
- UI widgets bind to these delegates in Blueprint
- Keeps C++ UI-agnostic
- Examples: `OnNotebookEntryAdded`, `OnInteractableDetected`, `OnRoomFlipProgress`

**StateTree AI:**
- AI behavior defined in StateTree assets, not hard-coded
- Controller provides infrastructure (perception, targeting)
- Utility functions in `ShooterStateTreeUtility` support StateTree tasks

## Input System

**Enhanced Input System:**
- Input Actions defined per variant (IA_Sprint, IA_Shoot, IA_SwapWeapon)
- Input Mapping Contexts: IMC_Horror, IMC_Weapons
- Base character provides `DoMove()`, `DoAim()`, `DoJump*()` virtuals
- Derived classes override and add variant-specific input handlers

## Common Patterns

**Fixed-tick systems:**
- Sprint stamina updates use `FTimerHandle` with fixed interval (`SprintFixedTickTime`)
- Weapon refire uses timers for full-auto mode

**Deferred destruction:**
- NPCs use `DeferredDestructionTime` timer before destroying after death
- Player characters use `RespawnTime` timer

**Aim calculation:**
- Characters provide target location via `GetWeaponTargetLocation()`
- Weapons apply aim variance cone to target direction
- NPCs add vertical offset and random variance for realistic AI aiming

## Testing

**Manual testing in editor:**
- PIE (Play In Editor) for quick iteration
- Test horror variant in `Lvl_Horror`
- Test shooter variant in `Lvl_Shooter`
- Default map: `Lvl_FirstPerson`

**Blueprint integration:**
- Most concrete implementations are Blueprints
- C++ provides abstract base classes and core systems
- Test gameplay in Blueprints: BP_HorrorCharacter, BP_ShooterCharacter, BP_ShooterNPC

## Room Flip Puzzle System (Horror Variant)

**Complete Puzzle Workflow:**

1. **Room Setup** (`BP_RoomFlip_compartment` based on `ARoomFlipActor`):
   - Place in level as the rotating room/train car
   - Configure rotation axis (X, Y, or Z)
   - Set rotation angle (default 180°)
   - Adjust duration and easing curve
   - Enable player attachment to keep player in room during flip

2. **Trigger Setup** (`BP_RoomFlipTrigger_v2` based on `AInteractableTrigger`):
   - Place in level near the room
   - In **Details** panel, assign **Room To Flip** variable to your room instance
   - Configure **Interaction Prompt Text** (e.g., "Press E to Flip Room")
   - Set **Single Use** if one-time flip is desired
   - Ensure **InteractionBox** component blocks **Visibility** channel

3. **Collectible Placement** (`BP_Collectible_Note` based on `ACollectibleActor`):
   - Place on ceiling of room (300-400cm high)
   - Configure **Notebook Entry** with unique **Entry ID**
   - Player cannot reach it (InteractionDistance = 120cm)
   - After room flips 180°, collectible is now on floor and reachable

4. **Blueprint Event Graph** (in `BP_RoomFlipTrigger_v2`):
   ```
   Event On Triggered (from InteractableTrigger)
   │
   ├─→ Get Variable: RoomToFlip
   ├─→ Is Valid? (validation check)
   │   ├─ Is Valid → Trigger Flip (on RoomToFlip)
   │   └─ Is Not Valid → Print String "Room not assigned!"
   ```

**Key Blueprint Assets:**
- `BP_HorrorCharacter_Demo`: Main player character (in `Content/Demo/Blueprints/`)
- `BP_RoomFlip_compartment`: Your room flip actor instance
- `BP_RoomFlipTrigger_v2`: Trigger to activate room flip
- `BP_Collectible_Note`: Collectible notes that add to notebook

## Common Issues and Solutions

**Interaction Not Working:**
- **Check Collision**: Ensure interactable actor has collision component that **blocks Visibility channel**
  - InteractionBox or StaticMesh must have: Collision Enabled = Query Only, Visibility = Block
- **Check Interface**: Verify Blueprint has **Interactable** interface added in Class Settings → Interfaces
- **Check CanInteract**: Event Can Interact must exist and return **True**
- **Check Distance**: Default interaction range is 120cm - place objects within reach or adjust InteractionDistance in BP_HorrorCharacter_Demo

**BlueprintNativeEvent Crashes:**
- **Never call interface functions directly** - always use `Execute_FunctionName()` from external calls
- **Within implementing class**, call `FunctionName_Implementation()` instead of `FunctionName()`
- Example: `OnInteract_Implementation` calls `CanInteract_Implementation` (NOT `CanInteract`)

**Room Flip Not Triggering:**
- **Check Variable Assignment**: In level, select trigger instance → Details → ensure **Room To Flip** is assigned
- **Add Null Check**: Use "Is Valid?" node before calling Trigger Flip
- **Check Blueprint Parent**: Trigger must inherit from `InteractableTrigger` (NOT plain Actor)

**Collectible Entry ID Errors:**
- Always set **Entry ID** in Blueprint defaults (never leave as "None")
- Entry IDs must be unique across all collectibles
- Check Output Log for validation warnings

**Input Not Responding:**
- Verify **Input Actions** are assigned in BP_HorrorCharacter_Demo Class Defaults:
  - Move Action, Look Action, Jump Action, Sprint Action, Interact Action, Toggle Notebook Action
- Check **Input Mapping Context** is added in PlayerController BeginPlay:
  - IMC_Default (for movement) with priority 0
  - IMC_Horror (for sprint/interact) with priority 1
- Verify correct pawn is spawned: Check Game Mode → Default Pawn Class = BP_HorrorCharacter_Demo

**Debug Visualization:**
- Enable debug line in `HorrorCharacter.cpp:229` to see interaction raycast:
  - Uncomment `DrawDebugLine()` to see green (hit) or red (miss) line
  - Uncomment `DrawDebugSphere()` to see exact hit point
- Check Output Log for interaction messages (added temporarily during debugging)
