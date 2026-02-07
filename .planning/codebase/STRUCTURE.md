# Codebase Structure

**Analysis Date:** 2026-02-01

## Directory Layout

```
Source/
├── control/                 # UI panel state and control logic
├── controls/                # Input handling (keyboard, controller, touch)
├── data/                    # Game data parsing and loaders
├── DiabloUI/                # Main menu and UI dialogs
├── discord/                 # Discord Rich Presence integration
├── dvlnet/                  # Network abstraction layer
├── engine/                  # Core engine systems (rendering, assets, events)
├── engine/render/           # Rendering backends and effects
├── items/                   # Item validation
├── levels/                  # Dungeon generation and level data
├── lua/                     # Lua scripting support
├── monsters/                # Monster validation
├── mpq/                     # MPQ file reading (asset archives)
├── panels/                  # In-game UI panels (inventory, spellbook, etc.)
├── platform/                # Platform-specific code (Android, iOS, Switch, etc.)
├── portals/                 # Town portal and level transition logic
├── qol/                     # Quality-of-life features (optional enhancements)
├── quests/                  # Quest state and progression
├── storm/                   # Legacy Storm networking wrapper
├── tables/                  # Static game data (spells, monsters, items, players)
├── utils/                   # Utility functions and helpers
│
├── automap.cpp/h            # Minimap rendering and interaction
├── appfat.cpp/h             # Fatal error handling
├── capture.cpp/h            # Screenshot functionality
├── codec.cpp/h              # Audio codec utilities
├── crawl.cpp/hpp            # Dungeon layout crawling for pathfinding
├── cursor.cpp/h             # Cursor sprite and positioning
├── dead.cpp/h               # Death screen and respawn
├── debug.cpp/h              # Debug console and visualization
├── diablo.cpp/h             # Main game loop and event dispatcher
├── diablo_msg.cpp/hpp       # Multiplayer messaging
├── doom.cpp/h               # Doom screen (end-game event)
├── effects.cpp/h            # Particle effects and animations
├── encrypt.cpp/h            # Network packet encryption
├── gamemenu.cpp/h           # Pause menu
├── gmenu.cpp/h              # Game menu overlay
├── help.cpp/h               # Help screen
├── hwcursor.cpp/h           # Hardware cursor support
├── init.cpp/h               # Game initialization
├── interfac.cpp/h           # Interface/UI state management
├── inv.cpp/h                # Inventory management (73KB)
├── items.cpp/h              # Item system (151KB)
├── lighting.cpp/h           # Light propagation and rendering
├── loadsave.cpp/h           # Save/load game functions
├── menu.cpp/h               # Main menu
├── minitext.cpp/h           # Small text rendering (item descriptions)
├── missiles.cpp/h           # Projectile system
├── movie.cpp/h              # Cinematic playback
├── multi.cpp/h              # Multiplayer state
├── nthread.cpp/h            # Game loop threading and timing
├── objects.cpp/h            # Interactive objects (doors, chests, etc.)
├── options.cpp/h            # Game settings and config
├── pfile.cpp/h              # Player save files
├── plrmsg.cpp/h             # Player message display
├── quick_messages.cpp/h     # Quick chat messages
├── restrict.cpp/h           # Player/item restrictions
├── spells.cpp/h             # Spell definitions
├── stores.cpp/h             # NPC trading interface
├── textdata.cpp/h           # Text messages
├── tradehealth.cpp/h        # Healer and witch trading
├── player.cpp/h             # Player state and mechanics (103KB)
├── monster.cpp/h            # Monster AI and behavior (161KB)
├── world.cpp/h              # World state
└── CMakeLists.txt           # Build configuration (Source subdirectory)
```

## Directory Purposes

**Source/control/:**
- Purpose: Main UI control flow and panel management
- Contains: Panel state globals (CharPanelButtonActive, MainPanelFlag, etc.), rectangle definitions for clickable areas
- Key files: `control.hpp` (panel state), `control.cpp` (panel opening/closing)

**Source/controls/:**
- Purpose: Input event translation to game actions
- Contains: Keymapper (keyboard→action mapping), controller handling, touch screen, menu controls
- Key files: `plrctrls.cpp` (largest, 67KB - player control handling), `keymapper.cpp`, `controller_buttons.cpp`
- Devices: Subdirectory `devices/` for platform-specific input drivers

**Source/data/:**
- Purpose: Parse and load text-based game data (not MPQ archives)
- Contains: Parser framework, record readers, value readers
- Key files: `parser.cpp/h`, `record_reader.cpp/h`
- Used by: Table loading in `Source/tables/`

**Source/DiabloUI/:**
- Purpose: Main menu and pre-game UI
- Contains: Main menu, character selection, hero creation, difficulty selection, multiplayer setup
- Key files: `diabloui.cpp` (main UI flow), `hero/` (character selection screen), `multi/` (multiplayer dialogs)

**Source/engine/:**
- Purpose: Low-level engine abstractions (rendering, assets, I/O, events)
- Contains: Asset management, sprite loading, animation info, random number generation, event handling
- Render subdirectory: CLX rendering, dungeon rendering, lighting, text rendering, primitive shapes
- Key files: `assets.hpp` (asset loading), `dx.h` (SDL abstraction), `animationinfo.cpp/h`, `events.cpp/h`

**Source/engine/render/:**
- Purpose: Rendering pipelines and effects
- Contains: Sprite rendering (clx_render), dungeon tile rendering (dun_render), lighting (light_render), text rendering, primitives
- Key files: `scrollrt.cpp` (63KB - main viewport rendering), `dun_render.cpp` (53KB - dungeon drawing), `text_render.cpp` (33KB)

**Source/items/:**
- Purpose: Item data validation
- Contains: Integrity checks for item tables
- Key files: `validation.cpp/h` - validate item properties match expected ranges

**Source/levels/:**
- Purpose: Dungeon generation, themes, special maps
- Contains: Procedural generation algorithms for each dungeon level, preset maps (unique maps), room theming
- Key files: `drlg_l*.cpp` (dungeon generators L1-L4), `gendung.cpp/h` (main API), `town.cpp` (Tristram), `themes.cpp` (room themes)
- Generated: Each level created fresh or loaded from seed

**Source/lua/:**
- Purpose: Lua scripting support for mods
- Contains: Lua binding, global functions exposed to scripts
- Key files: `lua_global.cpp/h`

**Source/monsters/:**
- Purpose: Monster data validation
- Contains: Integrity checks for monster tables
- Key files: `validation.cpp/h`

**Source/mpq/:**
- Purpose: MPQ archive reading
- Contains: MPQ decompression and file extraction
- Key files: `mpq_reader.cpp/h`

**Source/panels/:**
- Purpose: In-game UI panels (character, inventory, spellbook, etc.)
- Contains: Panel rendering, interaction handlers
- Key files: `spell_book.cpp`, `info_box.cpp`, `partypanel.cpp`, `console.cpp` (debug)
- Used by: Toggled by control panel globals in `control/control.hpp`

**Source/platform/:**
- Purpose: Platform-specific code
- Contains: Android JNI, iOS entry point, console platform code (Switch, Vita, etc.), locale handling
- Key files: `locale.cpp/h` (language detection)

**Source/portals/:**
- Purpose: Level transitions, town portals, dungeon entering
- Contains: Portal logic, level load triggers
- Key files: `portals.cpp/h`

**Source/qol/:**
- Purpose: Quality-of-life optional features
- Contains: Floating damage numbers, item labels, monster health bar, chat log, stash, XP bar, auto-pickup
- Design: Pluggable features that don't affect core mechanics, toggled via config
- Key files: `stash.cpp` (20KB - stash storage system), `floatingnumbers.cpp`, `itemlabels.cpp`

**Source/quests/:**
- Purpose: Quest progression and special events
- Contains: Quest state tracking, quest completion handlers
- Key files: `quests.cpp/h`

**Source/storm/:**
- Purpose: Legacy Storm networking wrapper
- Contains: Network packet definitions, Storm library abstractions
- Key files: `storm_net.cpp/h`, `storm_svid.cpp` (video/streaming)

**Source/tables/:**
- Purpose: Static game data - definitions for all game content
- Contains: Spell properties, monster stats, item properties, player base stats
- Key files: `spelldat.cpp/h`, `monstdat.cpp/h`, `itemdat.cpp/h`, `playerdat.cpp/h`, `misdat.cpp/h`, `objdat.cpp/h`
- Design: Each table loaded once at startup, then indexed by ID during gameplay

**Source/utils/:**
- Purpose: Reusable utilities and helpers
- Contains: File I/O, string utilities, math, logging, display management, encoding/decoding (CEL, CL2, CLX)
- Key files: `display.cpp/h` (SDL display initialization), `file_util.cpp/h`, `log.hpp`, `language.cpp/h`
- Subdirectories: `algorithm/` (pathfinding, line drawing)

## Key File Locations

**Entry Points:**
- `Source/diablo.cpp` - Main game loop, event dispatcher, primary orchestrator (92KB)
- `Source/diablo.h` - Game loop state, enums (GameLogicStep, PlayerActionType, click type), global variables

**Configuration:**
- `Source/options.cpp/h` - Game settings (graphics, audio, gameplay options)
- `Source/engine/assets.hpp` - Asset system configuration and file loading

**Core Logic:**
- `Source/player.cpp/h` - Player entity and mechanics (103KB, 30KB header)
- `Source/monster.cpp/h` - Monster entity and AI (161KB, 17KB header)
- `Source/items.cpp/h` - Item system (151KB, 16KB header)
- `Source/missiles.cpp/h` - Projectiles and spell effects
- `Source/objects.cpp/h` - Interactive world objects
- `Source/levels/gendung.cpp/h` - Level generation and management

**Rendering:**
- `Source/engine/render/scrollrt.cpp` - Main viewport rendering (63KB)
- `Source/engine/render/dun_render.cpp` - Dungeon tile drawing (53KB)
- `Source/engine/render/light_render.cpp` - Lighting calculations (24KB)
- `Source/engine/render/text_render.cpp` - Text rendering (32KB)
- `Source/engine/clx_sprite.hpp` - Sprite loading and blitting

**Testing:**
- No dedicated `test/` directory visible; tests likely in separate build with `BUILD_TESTING=ON`

## Naming Conventions

**Files:**
- `.cpp` for implementation, `.h` for C-style headers, `.hpp` for C++ headers
- Compound words use underscores: `diablo_msg.cpp`, `player_graphic`
- Paired files same name: `player.cpp` + `player.h`

**Functions:**
- Lowercase with underscores: `app_fatal()`, `InitKeymapActions()`, `game_loop()`
- Abbreviations common: `pcurs` (player cursor position), `gbRunGame` (global bool run game)
- Getters/setters: `GetMainPanel()`, `SetCursorPos()`
- Query predicates: `IsPlayerInStore()`, `HaveHellfire()`

**Variables:**
- Global prefixes: `gb` (global bool), `g` (global), `sg` (static global)
- Local: standard camelCase or lowercase
- Constants: `NUMLEVELS`, `InventoryGridCells` (PascalCase or UPPER_CASE)
- Member variables: `_pHitPoints`, `_oBreak` (underscore prefix, single letter for data type)

**Types:**
- Enums: PascalCase or lowercase with underscores: `GameLogicStep`, `inv_body_loc`
- Structs: PascalCase: `Player`, `Monster`, `Item`
- Aliases: `using MpqArchiveT = MpqArchive`

**Include Guards:**
- `#pragma once` (modern, used throughout)

## Where to Add New Code

**New Feature (Core Gameplay):**
- Primary code: `Source/[feature_name].cpp` and `Source/[feature_name].h`
- If complex: Create subdirectory `Source/[feature_name]/` with `feature.cpp/h` inside
- Example: New quest feature → `Source/quests/` already exists
- Register with game loop: Add to `GameLogicStep` enum if processing order matters

**New Component/Module:**
- Implementation: `Source/[subsystem]/[component].cpp` and `.h`
- If UI: Place in `Source/panels/` or `Source/DiabloUI/`
- If cross-platform: Platform-specific in `Source/platform/[feature]/`
- Example: New monster type validation → Add to `Source/monsters/validation.cpp`

**Utilities:**
- Shared helpers: `Source/utils/[utility_name].cpp/h`
- Algorithm: `Source/utils/algorithm/[algorithm].hpp`
- Example: New math function → `Source/utils/math.h`

**Data Tables:**
- Static game data: `Source/tables/[datatype]dat.cpp/h`
- Parser framework exists; follow `itemdat.cpp` pattern
- Data files loaded via `Source/data/parser.cpp`

**Platform-Specific:**
- Android: `Source/platform/android/`
- iOS: `Source/platform/ios/`
- Console: `Source/platform/[console]/`
- Shared: `Source/platform/locale.cpp`

## Special Directories

**Source/3rdParty/:**
- Purpose: External dependencies (included in repo or vendored)
- Generated: No (committed to repo)
- Committed: Yes, vendored libraries

**build/:**
- Purpose: CMake build artifacts
- Generated: Yes
- Committed: No (in .gitignore)

**assets/:**
- Purpose: Game assets (graphics, fonts, audio, data)
- Generated: Some (CEL→CLX conversion, translations)
- Committed: Yes (critical assets)

**docs/:**
- Purpose: User documentation, building guides, changelog
- Generated: No
- Committed: Yes

---

*Structure analysis: 2026-02-01*
