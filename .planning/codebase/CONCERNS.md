# Codebase Concerns

**Analysis Date:** 2026-02-01

## Tech Debt

### Missile Damage State Fragility

**Issue:** Missile damage values are derived from player/monster state at time of impact, not encoded in the missile struct itself. This creates vulnerability to crashes if the source entity (player or monster) dies or leaves the game before the missile reaches its target.

**Files:**
- `Source/missiles.cpp` (lines 2862-2949, 2889-2950, 3354-3610)

**Impact:**
- Potential null pointer dereference or out-of-bounds array access in multiplayer when:
  - A player fires a missile, then disconnects before impact
  - A monster fires a missile, then dies before the missile reaches target
  - Trap missiles with invalid source references
- Multiple comments note this issue is a BUGFIX (lines 2862, 2867, 2889, 2902, 2943, 2949, 3354, 3357, 3605, 3610)

**Fix approach:**
1. Add damage fields to the Missile struct (`_midmg_min`, `_midmg_max`)
2. Capture damage at missile creation time in functions like `AddMissile()`
3. Replace all `Players[missile._misource]` and `Monsters[missile._misource]` reads with stored values
4. Requires serialization changes for save/load compatibility

---

### Item Placement Fallback

**Issue:** `GetSuperItemLoc()` returns default position `{0, 0}` when no valid drop location is found, potentially placing items at level boundaries or other invalid positions.

**Files:** `Source/items.cpp` (line 3114)

**Impact:**
- Items may be placed in inaccessible or invalid world positions
- Could cause visual glitches or gameplay issues if items are unreachable
- TODO comment explicitly acknowledges the problem

**Fix approach:**
1. Return `std::optional<Point>` instead of fallback
2. Handle missing space in calling code (e.g., add to player inventory or drop at player location as secondary fallback)
3. Audit all callers of `GetSuperItemLoc()`

---

### Infinite Item Generation Loop

**Issue:** `GetPremiumItem()` can hang indefinitely in non-Hellfire mode if no suitable item can be generated within price constraints.

**Files:** `Source/items.cpp` (line 1951)

**Impact:**
- Game freeze when trying to generate premium items (shops) if item generation algorithm can never satisfy value constraints
- Only affects vanilla Diablo (non-Hellfire), but the condition is checked every premium item generation

**Fix approach:**
1. Add reasonable maximum iteration count as fallback
2. Return null/optional item if limit exceeded, with graceful degradation (shop shows fewer items)
3. Log warning when this occurs for debugging

---

### Uninitialized Variable in Dungeon Generation

**Issue:** Variable `pdir` is uninitialized in dungeon generation code. While marked as fixed, indicates previous fragility.

**Files:** `Source/levels/drlg_l3.cpp` (line 1022)

**Impact:**
- Code path now has proper initialization, but demonstrates lack of safeguards in dungeon generation
- Similar patterns may exist elsewhere

**Fix approach:**
1. Use compiler flags to catch uninitialized variables (`-Wuninitialized`)
2. Prefer language features (structured bindings, initialization lists) over bare declarations

---

### Multiple Workarounds for Dungeon Generation Bugs

**Issue:** Multiple BUGFIX comments indicate dungeon generation contains known bugs that are worked around rather than fixed to maintain compatibility.

**Files:**
- `Source/levels/drlg_l1.cpp` (lines 353, 475, 832-1043)
- `Source/levels/drlg_l2.cpp` (lines 1715, 2468-2511)
- `Source/levels/drlg_l3.cpp` (lines 1022-1751, 1373, 1403, 1477)
- `Source/levels/drlg_l4.cpp` (lines 231, 1020-1025)

**Impact:**
- Swapped height/width dimensions in room size checks
- Off-by-one loop bounds causing potential out-of-bounds access
- Skipped or incorrect bounds checks before dungeon array access
- Maintains vanilla Diablo compatibility but prevents cleaner refactoring

**Fix approach:**
1. Document which bugs exist purely for save format compatibility vs. gameplay correctness
2. Create separate "clean" implementation for new content
3. Isolate compatibility hacks into clearly-marked sections with version checks

---

## Known Bugs

### Dialogue Cross-Reference Coupling

**Issue:** UI dialogs refer directly to each other's static variables, breaking encapsulation and making it difficult to test or modify dialogs independently.

**Files:**
- `Source/DiabloUI/multi/selgame.cpp` (line 460)
- `Source/DiabloUI/hero/selhero.cpp` (line 375)

**Symptoms:**
- Difficulty selection dialog reads `selhero_isMultiPlayer` and writes to `selhero_endMenu`
- Changes to one dialog's internal state affect other dialogs unpredictably
- Single-player difficulty selection hijacks hero selection menu

**Workaround:** Comments clearly mark this as "dangerous hack"

**Fix approach:**
1. Create shared dialog state object rather than cross-referencing globals
2. Use event callbacks or observer pattern for dialog communication
3. Separate single-player and multiplayer difficulty selection into distinct code paths

---

### Missile Y-Coordinate Bug

**Issue:** Monster placement code adds Y displacement twice using wrong field (uses `deltaX` for both X and Y).

**Files:** `Source/monster.cpp` (line 335)

**Symptoms:**
- Monsters spawned from groups place at incorrect coordinates
- Comment indicates this is a fixed BUGFIX

**Status:** Already fixed in current code, but demonstrates need for better type safety

**Fix approach:**
1. Use structured types (`WorldTilePosition` or `Point`) instead of separate x/y variables
2. Compiler will catch misuse of `.deltaX` when `.deltaY` intended

---

### Shop Voice Playing Incorrectly

**Issue:** Quest item voice lines may be incorrect in multiplayer when same quest item picked up by different players.

**Files:** `Source/inv.cpp` (line 1000)

**Symptoms:**
- Mushroom quest voice line may be spoken incorrectly when picked up in MP
- No verification of voice ID correctness across network

**Fix approach:**
1. Use gender-aware voice selection
2. Send voice ID over network rather than deriving client-side
3. Add validation that voice enum is valid before playback

---

### Ambiguous Item Spawn Logic

**Issue:** Treasure spawning code has unclear intent with disconnected logic branches.

**Files:** `Source/levels/themes.cpp` (lines 525-543)

**Symptoms:**
- Gold may be spawned (line 526-528)
- Random item may be spawned (line 530-532)
- Item value halving happens regardless of which items were created (line 538-542)
- Comment marks as "likely not working as intended"

**Impact:**
- Last spawned item (unrelated to treasure logic) gets halved if `rv >= treasureType - 2`
- Creates unexpected value variation in loot

**Fix approach:**
1. Refactor to clarify intent: which items qualify for value reduction?
2. Ensure value halving only applies to intended item types
3. Add documentation of treasure spawn probabilities

---

### Spell Voice Cutoff (Unimplemented)

**Issue:** Multiple simultaneous speech requests may queue or play overlapping, no prevention implemented.

**Files:** `Source/player.h` (line 526)

**Impact:**
- Auditory feedback becomes garbled in rapid-cast scenarios
- BUGFIX todo indicates this should be prevented

**Fix approach:**
1. Track active speech with timestamp
2. Reject new requests if speech currently playing
3. Return boolean indicating success/rejection

---

## Performance Bottlenecks

### Data File Streaming Not Implemented

**Issue:** Data file contents are entirely loaded into memory via `std::unique_ptr<char[]>` allocation. Large game data structures copied entirely on load.

**Files:** `Source/data/file.cpp` (line 23)

**Impact:**
- High memory usage on resource-constrained platforms (embedded, mobile, consoles)
- No capability to stream large assets
- Save/load times proportional to total file size

**Improvement path:**
1. Implement lazy loading for non-critical assets
2. Add streaming decompression for compressed data
3. Use memory-mapped file I/O on platforms that support it
4. Profile actual memory impact on target platforms

---

### Game Data Loaded Too Early

**Issue:** Spell, missile, and monster data loaded in `diablo.cpp` initialization before game actually starts.

**Files:** `Source/diablo.cpp` (line 2751)

**Impact:**
- Main menu load time includes data table initialization
- All spell/missile/monster data loaded even for single-player when most may not be needed
- Startup memory footprint unnecessary during menus

**Improvement path:**
1. Defer loading to first dungeon entry
2. Lazy-load per-level data (don't load L16 data until Nightmare)
3. Profile impact on perceived responsiveness

---

### Large Monolithic Files

**Issue:** 13 source files exceed 2000 lines, with largest files being core game logic.

**Files:**
- `Source/items.cpp` (5,054 lines)
- `Source/monster.cpp` (4,988 lines)
- `Source/objects.cpp` (4,927 lines)
- `Source/missiles.cpp` (4,286 lines)

**Impact:**
- Difficult to navigate and understand
- Higher chance of unintended interactions between functions
- Compilation slow for these files
- Increased cognitive load for contributors

**Improvement path:**
1. Split by subsystem (e.g., item drops vs. item attributes vs. item effects)
2. Extract utility functions to separate files
3. Create clear module boundaries with explicit interfaces
4. Set target of <1500 lines per file

---

## Fragile Areas

### Dungeon Generation Layer

**Files:**
- `Source/levels/drlg_l1.cpp` (1,346 lines)
- `Source/levels/drlg_l2.cpp` (2,853 lines)
- `Source/levels/drlg_l3.cpp` (2,219 lines)
- `Source/levels/drlg_l4.cpp` (1,250 lines)

**Why fragile:**
- Tightly coupled dungeon layout generation with array bounds
- Many out-of-bounds access bugs that required bounds-checking fixes
- Coordinate system assumptions (height/width swaps) throughout
- No clear separation between room generation, hallway generation, and decoration

**Safe modification:**
1. Add bounds checking wrapper around all dungeon array accesses
2. Use `std::array<std::array<int, DMAXY>, DMAXX>` with .at() for safe bounds checking
3. Create coordinate type (`DungeonCoord` or `TileCoord`) to prevent swaps
4. Test all level generation extensively after any change

**Test coverage:** Level generation tests exist but may not cover edge cases (small rooms, unusual random seeds)

---

### Keymapper Input Handling

**Files:** `Source/controls/keymapper.cpp` (lines 59, 71)

**Why fragile:**
- TODO comments indicate chat flag checking doesn't belong in keymapper
- Mixed concerns: input mapping vs. game state filtering
- Coupled to global `ChatFlag` and `DropGoldFlag` variables

**Safe modification:**
1. Refactor to pass input context (game state) as parameter
2. Create `InputContext` struct with flags
3. Let keymapper only handle key-to-action mapping, not filtering
4. Let caller decide whether action should execute based on game state

**Test coverage:** Gaps in testing input state interactions

---

### Item Value Type Mismatch

**Issue:** Item values are signed `int` but prices should be unsigned. Casting between types creates risk.

**Files:** `Source/stores.cpp` (line 408)

**Impact:**
- Negative prices could theoretically occur
- Comparison logic may have unexpected behavior with signed arithmetic

**Safe modification:**
1. Change `_iIvalue` to `uint32_t`
2. Update all assignments to prevent negative initialization
3. Update price comparison functions accordingly
4. Change function signature to accept `uint32_t price`

---

### Lighting Array Duplication

**Issue:** Lighting array `dLight` is saved twice in save file, indicating past bugs or confusion about what should be saved.

**Files:** `Source/loadsave.cpp` (line 2900)

**Impact:**
- Save files include redundant data
- If trying to fix this, must maintain compatibility with existing saves
- Code comment indicates confusion about what gets saved

**Safe modification:**
1. Check if dLight actually appears twice in load code
2. If so, document why (compatibility hack?)
3. If not, investigate if second write is erroneous
4. Create migration script if fixing

---

## Security Considerations

### No Input Validation on Item Drop Positions

**Issue:** Item drop position validation uses `FindClosestValidPosition()` but fallback is silent default position.

**Files:** `Source/items.cpp` (line 3114)

**Risk:**
- Items placed at `{0, 0}` bypass intended drop location logic
- Could be exploited in multiplayer to generate items at level boundaries

**Current mitigation:** Presumably dungeon structure prevents actual issues, but not explicit

**Recommendations:**
1. Return optional and require caller to handle placement failure
2. Add assertions/logging when fallback is used
3. Audit all item spawn locations for security implications

---

### Monster/Player Array Bounds Assumptions

**Issue:** Code frequently accesses `Players[]` and `Monsters[]` arrays using indices from game state without bounds checking in all cases.

**Files:**
- `Source/missiles.cpp` (throughout, 2860+ accesses)
- `Source/monster.cpp` (350+ accesses)
- `Source/player.cpp` (100+ accesses)

**Risk:**
- If `_misource` or `_midist` are corrupted or invalid, leads to buffer overflow
- Network messages could provide invalid indices

**Current mitigation:** Some caching and assertions, but not comprehensive

**Recommendations:**
1. Create safe indexing wrapper: `SafePlayer(int index)` returning `optional<Player&>`
2. Add bounds checking at message deserialization layer
3. Use static_assert for array size assumptions

---

## Missing Critical Features

### No Global State Cleanup on Game Reset

**Issue:** Multiple global arrays and state variables persist across game sessions without explicit reset in all cases.

**Files:** Throughout, especially `Source/diablo.cpp`

**Problem:**
- If game initialization fails partway, cleanup may be incomplete
- Transitioning between single-player and multiplayer may leave stale state

**Blocks:** Robust error recovery, feature parity with original Diablo

---

### No Streaming Asset Loading

**Issue:** All assets loaded into memory simultaneously, no capability to load/unload per-level.

**Files:** `Source/data/file.cpp` and asset loading layer

**Blocks:**
- Support for very large mods
- Console/embedded platform optimization
- Efficient memory management on resource-constrained devices

---

## Test Coverage Gaps

### Dungeon Generation Edge Cases

**What's not tested:**
- Level generation with unusual random seeds
- All room size combinations
- Out-of-bounds access prevention for all 4 level types
- Compatibility with saves from vanilla Diablo

**Files:** `Source/levels/drlg_l*.cpp`

**Risk:**
- Regression when modifying generation logic
- Edge case crashes in uncommon level layouts

**Priority:** High

---

### Missile Physics and Collision

**What's not tested:**
- Missile hitting target that becomes invalid during flight
- Multiple missiles from same caster in flight simultaneously
- Monster missiles vs. player missiles vs. trap missiles with edge cases

**Files:** `Source/missiles.cpp`

**Risk:**
- Crashes in multiplayer with complex spell sequences
- Data corruption from invalid array access

**Priority:** High

---

### Item Generation and Drop Logic

**What's not tested:**
- Premium item generation with all value constraints
- Item drop when inventory full
- Item drop when no valid dungeon position exists
- Quest item special handling across multiplayer

**Files:** `Source/items.cpp`

**Risk:**
- Infinite loops during shop initialization
- Items placed in invalid locations
- Voice/quest state mismatches

**Priority:** Medium

---

### Input State Transitions

**What's not tested:**
- Rapid keymapper input during chat
- Input filtering with multiple active flags
- Joystick/controller input with keymapper configured

**Files:** `Source/controls/keymapper.cpp`

**Risk:**
- Unexpected input behavior in edge cases
- Difficult to debug in real gameplay

**Priority:** Low to Medium

---

*Concerns audit: 2026-02-01*
