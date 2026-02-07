# Architecture

**Analysis Date:** 2026-02-01

## Pattern Overview

**Overall:** Client-server action RPG engine with decoupled game logic, rendering, and networking layers.

**Key Characteristics:**
- **Turn-based game loop** with configurable update rate (500ms ticks for game logic)
- **Separate input processing** that can run at higher frequency than game logic
- **Abstracted rendering system** supporting SDL1, SDL2, and SDL3 backends
- **Network-agnostic gameplay** - same logic runs single/multiplayer with network abstraction layer
- **Asset-driven configuration** - spell, monster, item, player data loaded from MPQ archives or loose files

## Layers

**Engine/Rendering Layer:**
- Purpose: Low-level graphics, sprite rendering, palette management, text rendering
- Location: `Source/engine/`, `Source/engine/render/`
- Contains: Sprite/CEL/CLX loaders, viewport management, lighting calculations, CLX rendering, text rendering, asset management
- Depends on: SDL (1, 2, or 3), libc standard library
- Used by: UI system and main game loop for all drawing operations
- Key files: `dx.h` (backend abstraction), `assets.hpp` (asset loading), `clx_render.hpp`, `dun_render.cpp`, `scrollrt.cpp`

**Game Loop Layer:**
- Purpose: Core game logic execution, game state progression
- Location: `Source/diablo.cpp` (entry point), `Source/diablo.h` (defines like `GameLogicStep`)
- Contains: Game loop execution, player/monster/object/missile/item processing phases
- Depends on: All gameplay systems below
- Used by: Main entry point, triggered every 500ms by `nthread_has_500ms_passed()`
- Key files: `diablo.cpp` (`game_loop()` function), enums: `GameLogicStep`, `PlayerActionType`

**Input/Control Layer:**
- Purpose: Convert input events (keyboard, controller, touch, mouse) into game commands
- Location: `Source/controls/`, `Source/control/`
- Contains: Keymapper, controller handling, touch controls, input device abstraction
- Depends on: SDL events, player state
- Used by: Main event loop in `diablo.cpp`, converts SDL_Event into game actions
- Key files: `controls/keymapper.cpp`, `controls/plrctrls.cpp` (67KB - large), `controls/controller_buttons.cpp`, `control/control.hpp` (UI state)

**Gameplay Systems Layer:**
- Purpose: Specific game mechanics and entity behaviors
- Location: Multiple root-level files and subdirectories
- Contains modular systems:
  - **Players**: `player.cpp/h` (103KB) - player state, attributes, equipment, leveling
  - **Monsters**: `monster.cpp/h` (161KB) - AI, movement, spawning, behavior
  - **Items**: `items.cpp/h` (151KB) - item creation, stats, dropping, inventory
  - **Spells**: `spells.cpp/h` - spell lookup and definitions
  - **Missiles**: `missiles.cpp/h` - projectile handling, spell effects
  - **Objects**: `objects.cpp/h` - doors, chests, levers, interactive world
  - **Quests**: `Source/quests/` - quest state, special events
  - **Portals**: `Source/portals/` - dungeon transitions, town portal
  - **Lighting**: `lighting.cpp/h` - light propagation, torch effects
  - **Effects**: `effects.cpp/h` - particle effects, animations
- Used by: Game loop's phase processor calls these systems sequentially
- Data-driven: All mechanics controlled via tables in `Source/tables/`

**Level/Dungeon Generation Layer:**
- Purpose: Procedural and preset level generation, tile management
- Location: `Source/levels/`
- Contains: Dungeon generation algorithms (L1-L4, crypt, town), room themes, tile properties
- Depends on: RNG from `engine/random.hpp`, item/monster placement
- Key files: `drlg_l*.cpp` (level generators), `gendung.cpp/h` (main API), `themes.cpp`, `town.cpp`, `setmaps.cpp`

**Networking Layer:**
- Purpose: Multiplayer synchronization, packet handling, protocol abstraction
- Location: `Source/dvlnet/` (abstract protocol), `Source/storm/` (Storm legacy wrapper)
- Contains: Network packet handling, TCP/ZeroTier abstraction, frame queuing, encryption
- Depends on: SDL networking or platform-specific networking libs
- Used by: Game logic issues `NetSendCmd*` calls that are routed through this layer
- Key files: `dvlnet/base.cpp/h` (main multiplayer), `dvlnet/packet.cpp/h`, `dvlnet/protocol_zt.cpp` (ZeroTier)

**UI/Panels Layer:**
- Purpose: Character panel, spell book, inventory, status display, menus
- Location: `Source/panels/`, `Source/DiabloUI/`
- Contains: Panel rendering, button handling, menu navigation, character info display
- Depends on: Control layer for input, render layer for drawing
- Key files: `control/control.hpp` (panel state), `panels/*.cpp` (specific panels), `DiabloUI/*.cpp` (main menus)

**Quality-of-Life Features:**
- Purpose: Optional enhancements separate from core gameplay
- Location: `Source/qol/`
- Contains: Floating numbers, item labels, monster health bar, chat log, auto-pickup, stash, XP bar
- Used by: Enabled via config, integrated into game loop rendering
- Design: Pluggable, don't affect core mechanics

**Data/Configuration Layer:**
- Purpose: Load and parse game data from MPQ archives and config files
- Location: `Source/data/`, `Source/tables/`
- Contains: Data file parsing, record readers, static tables with spell/monster/item definitions
- Key files: `data/parser.cpp/h`, `tables/*.cpp/h` (monstdat, itemdat, spelldat, playerdat, etc.)

## Data Flow

**Startup Sequence:**
1. `DiabloMain()` initializes SDL, loads config, initializes engine
2. Asset loading via `LoadCoreArchives()`, `LoadGameArchives()`
3. User selects game mode (new/load) via `DiabloUI`
4. Level generation or load via `LoadGameLevel()`
5. Game loop begins

**Game Loop (per 500ms tick):**
1. **ProcessInput()** - Convert pending SDL events to game commands
2. **ProcessGameMessagePackets()** - Handle multiplayer messages
3. **game_loop()** executes in phases (gGameLogicStep):
   - ProcessPlayers: Update player position, animation, spells
   - ProcessMonsters: AI, pathfinding, attacks
   - ProcessObjects: Door/lever/trap logic
   - ProcessMissiles: Projectile movement, collision
   - ProcessItems: Item timeout, floor drops
   - ProcessTowners: NPC movement/dialog
4. **Rendering pass:**
   - `DrawAndBlit()` calls viewport rendering
   - Monsters/players/items/effects drawn in depth order
   - UI panels overlaid
   - Frame displayed via SDL

**Input -> Command Flow:**
1. SDL_Event received in main event loop
2. Routed through control handlers (menus, UI text input first)
3. If gameplay active: `ProcessInput()` → `plrctrls_*()` → `KeymapperPress()`
4. Keymapper converts to game action (walk, attack, spell)
5. Action becomes `LastPlayerAction` or triggers `NetSendCmd*()`
6. In multiplayer: Command sent to other players; locally applied at next game tick

**Rendering Pipeline:**
1. **Viewport rendering** (`scrollrt_draw_game_screen()`)
   - Clear backbuffer
   - Light map generation (`lighting_render.cpp`)
   - Dungeon tiles rendered (`dun_render.cpp`)
   - Sprites composed in depth order (CEL/CLX sprites via `clx_render.cpp`)
   - Missiles, effects drawn
2. **UI overlay**
   - Panels (inventory, spellbook, character)
   - Health/mana bars
   - Floating damage numbers
   - Monster health bars (QoL)
3. **Blit to screen** - Final frame push to SDL surface

## Key Abstractions

**GameLogicStep Enum:**
- Purpose: Identifies which subsystem is executing each tick
- Location: `diablo.h` lines 47-57
- Used by: Debug logging, can disable specific phases for testing
- Pattern: Sequential phase execution prevents conflicts between subsystem updates

**PlayerActionType Enum:**
- Purpose: Describes what action player attempted last
- Location: `diablo.h` lines 59-69
- Examples: Walk, Attack, Spell, OperateObject
- Pattern: Used to validate command execution and prevent command queueing

**Player/Monster/Item Structs:**
- Purpose: Core entity data models
- Location: `player.h` (struct not visible in excerpt, but defined), `monster.h`, `items.h`
- Pattern: Global arrays (`Players[]`, `Monsters[]`, `Items[]`) indexed by entity ID
- State stored in: Position, animation frame, flags, resources (HP/mana)

**Asset System:**
- Purpose: Unified interface for loading CEL/CLX/CL2 sprites, sounds, music
- Location: `engine/assets.hpp`
- Pattern: `FindAsset()` locates file in MPQ or loose folder, `OpenAsset()` returns handle
- Abstraction: Hides difference between UNPACKED_MPQS (loose files) and compressed archives

**Network Abstraction (dvlnet):**
- Purpose: Hide transport layer (TCP vs ZeroTier vs loopback)
- Location: `dvlnet/abstract_net.h`, `dvlnet/base.cpp`
- Pattern: Game logic issues `NetSendCmd*()`, packet delivery abstracted
- Fallback: Loopback mode for single-player (same send/receive from same player)

**Control Panels State:**
- Purpose: Track which UI panels are open
- Location: `control/control.hpp` globals
- Examples: `MainPanelFlag`, `CharPanelButtonActive`, `SpellbookFlag`
- Pattern: Boolean flags toggled by input handlers, read by rendering/input logic

## Entry Points

**Main Entry Point:**
- Location: `Source/diablo.cpp:DiabloMain()`
- Triggers: Program startup via command-line args or hardcoded defaults
- Responsibilities:
  - Parse command-line options (--data-dir, --lang, --demo, etc.)
  - Initialize engine (SDL, audio, graphics backend)
  - Load user config from INI file
  - Show UI (menus, character select)
  - Call `StartGame()` to begin game loop

**Game Loop Entry Point:**
- Location: `Source/diablo.cpp:game_loop(bool bStartup)`
- Triggers: Called once per 500ms tick from main event loop
- Responsibilities:
  - Execute all gameplay logic phases
  - Update game state
  - Return whether screen needs redraw

**Event Handler:**
- Location: `Source/diablo.cpp:HandleMessage()`
- Triggers: SDL event received
- Routes to: Menu handlers, UI text input, gameplay input, console (debug)

## Error Handling

**Strategy:** Fail-fast with user-facing error dialogs via `app_fatal()` or `diablo_quit()`

**Patterns:**
- **File/Asset Errors**: `FailedToOpenFileError()` in `engine/assets.hpp` calls `app_fatal()` in non-headless mode
- **Network Errors**: Logged to console, may trigger disconnect
- **Validation Errors**: `items/validation.cpp`, `monsters/validation.cpp` check data integrity
- **Debug Mode**: `#ifdef _DEBUG` code enabled for additional checks
- **Assertions**: `assert()` throughout for internal invariants, only checked in debug builds

## Cross-Cutting Concerns

**Logging:**
- Approach: `utils/log.hpp` provides `log` macro
- Debug logging via `fmt::print()` to stdout/SDL log
- Optional file logging via `--log-to-file` CLI option
- SDL log callback can redirect to file in `diablo.cpp:SdlLogToFile()`

**Validation:**
- Approach: Separate validation modules in `items/`, `monsters/`
- Run on startup to check data file integrity
- Example: `items/validation.cpp:ValidateItems()` checks item tables

**Authentication:**
- Approach: Local only - no server authentication
- Multiplayer assumes LAN or trusted peer (Diablo/Hellfire model)
- Network encryption optional via `PACKET_ENCRYPTION` CMake option

**Pause/Resume:**
- Approach: `PauseMode` global (0=running, 1=ESC menu, 2=other modal)
- Game loop checks `if (PauseMode == 2) return false` to skip updates
- UI can set pause state when showing modals

**Demo Recording/Playback:**
- Approach: `engine/demomode.h` - optional feature compiled with `#ifndef DISABLE_DEMOMODE`
- Records input events and RNG seeds for deterministic replay
- Used for automated testing, benchmarking, regression detection

---

*Architecture analysis: 2026-02-01*
