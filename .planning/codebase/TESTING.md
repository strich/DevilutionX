# Testing Patterns

**Analysis Date:** 2026-02-01

## Test Framework

**Runner:**
- Google Test (gtest) - primary test framework
- Google Mock (gmock) - mocking framework
- Google Benchmark (benchmark) - performance benchmarking
- Config file: None explicitly, initialized via `testing::InitGoogleTest(&argc, argv)` in `test/main.cpp`

**Assertion Library:**
- Google Test assertions: `EXPECT_*`, `ASSERT_*` macros
- Google Mock matchers: `ElementsAre`, `ElementsAreArray`, etc.

**Run Commands:**
```bash
# Build tests (CMake configured with BUILD_TESTING=ON by default)
cmake --build build --target RUN_TESTS

# Run tests directly
./build/DevilutionX_test

# Run specific test
./build/DevilutionX_test --gtest_filter="PathTest.*"

# List available tests
./build/DevilutionX_test --gtest_list_tests
```

## Test File Organization

**Location:**
- Test files colocated in `test/` directory at project root
- Mirrors source structure conceptually but all in single directory
- Test fixtures separated from test implementations in same files

**Naming:**
- Format: `{module}_test.cpp` (e.g., `path_test.cpp`, `player_test.cpp`)
- Format: `{module}_benchmark.cpp` for performance tests (e.g., `path_benchmark.cpp`)
- Consistent suffix convention across all test files

**Structure:**
```
test/
├── main.cpp                    # Test runner entry point
├── Fixtures.cmake              # CMake configuration for test fixtures
├── fixtures/                   # Test data files
│   └── text_render_integration_test/
├── *_test.cpp                  # Unit tests
├── *_benchmark.cpp             # Performance benchmarks
├── player_test.h               # Shared test helpers
└── app_fatal_for_testing.cpp   # Test-specific stubs
```

## Test Structure

**Suite Organization:**
```cpp
#include <gtest/gtest.h>

namespace devilution {
namespace {

// Using anonymous namespace for test helpers and fixtures
class InvTest : public ::testing::Test {
public:
    void SetUp() override
    {
        // Per-test setup
        Players.resize(1);
        MyPlayer = &Players[0];
    }

    static void SetUpTestSuite()
    {
        // One-time suite setup
        LoadCoreArchives();
        LoadGameArchives();
    }

private:
    static bool missingMpqAssets_;
};

// Test implementation
TEST_F(InvTest, UseScroll_from_inventory)
{
    set_up_scroll(MyPlayer->InvList[2], SpellID::Firebolt);
    EXPECT_TRUE(CanUseScroll(*MyPlayer, SpellID::Firebolt));
}

} // namespace
} // namespace devilution
```

**Patterns:**
- `SetUpTestSuite()` static method: Called once per test class, loads resources
- `SetUp()` override: Called before each test, initializes test state
- `TEST_F()` macro: Test fixture-based tests with setup/teardown
- `TEST()` macro: Simple tests without fixtures
- Anonymous namespace wraps test helpers to avoid namespace pollution
- All tests wrapped in `devilution` namespace

## Mocking

**Framework:** Google Mock (gmock) integrated with gtest

**Patterns:**
```cpp
#include <gmock/gmock.h>

using ::testing::ElementsAre;
using ::testing::ElementsAreArray;

// Matcher usage
EXPECT_THAT(order, ElementsAre(
    Displacement(0, 0),
    Displacement(0, 1)
));

// Mock expectations
EXPECT_CALL(mockObj, Method).Times(::testing::AtLeastOnce());
```

**What to Mock:**
- External dependencies (file systems, networks) - use filesystem mocks where needed
- Expensive operations when testing other behavior - avoid if integration tests needed instead
- Complex state setup when mocking simplifies test

**What NOT to Mock:**
- Core game logic paths - use integration tests instead
- Data structures that are essential to test - construct real objects
- Behavior you're testing - defeats the purpose of the test
- Simple utility functions - call the real implementation

**Test Listeners:**
- Custom `SkippedTestListener` in `main.cpp` tracks and reports skipped tests
- Implements `testing::EmptyTestEventListener`
- Overrides `OnTestPartResult` to detect skips
- Overrides `OnTestProgramEnd` to print summary

## Fixtures and Factories

**Test Data:**
Test helper functions construct data for common scenarios:

```cpp
/* Set up a given item as a spell scroll, allowing for its usage. */
void set_up_scroll(Item &item, SpellID spell)
{
    pcurs = CURSOR_HAND;
    leveltype = DTYPE_CATACOMBS;
    MyPlayer->_pRSpell = static_cast<SpellID>(spell);
    item._itype = ItemType::Misc;
    item._iMiscId = IMISC_SCROLL;
    item._iSpell = spell;
}

/* Clear the inventory of MyPlayerId. */
void clear_inventory()
{
    for (int i = 0; i < InventoryGridCells; i++) {
        MyPlayer->InvList[i] = {};
        MyPlayer->InvGrid[i] = 0;
    }
    MyPlayer->_pNumInv = 0;
}
```

**Test Data Location:**
- Helper functions defined in test files directly (not in separate fixture libraries)
- Shared test utilities in `player_test.h` for player-related tests
- Data files in `test/fixtures/` directory (e.g., `fixtures/text_render_integration_test/`)
- TSV data files referenced via paths (e.g., `"txtdata\\cr.tsv"`)

**Const Test Data:**
```cpp
constexpr ItemSpecialEffect Normal = ItemSpecialEffect::None;
constexpr ItemSpecialEffect Balance = ItemSpecialEffect::FastHitRecovery;

struct BlockTestCase {
    int expectedRecoveryFrame;
    int maxRecoveryFrame;
    ItemSpecialEffect itemFlags;
};

BlockTestCase BlockData[] = {
    { 6, Warrior, Normal },
    { 7, Rogue, Normal },
};
```

## Coverage

**Requirements:** No enforced coverage minimums in CMake configuration

**View Coverage:** Not explicitly configured; coverage analysis would require external tooling

**Test Selection:**
- Some tests are skipped if MPQ assets unavailable: `GTEST_SKIP() << "MPQ assets not found"`
- Custom skip listener summarizes skip reasons at end of test run
- Critical tests check for missing resources and report clearly

## Test Types

**Unit Tests:**
- Scope: Individual functions and classes
- Approach: Test single responsibility; mock external dependencies
- Examples: `PathTest::Heuristics`, `RandomTest::RandomEngineParams`, `DataFileTest::LoadCRFile`
- Pattern: Direct function calls with known inputs and validated outputs

**Integration Tests:**
- Scope: Multiple components working together
- Approach: Load real data (MPQ archives), initialize systems, verify interactions
- Examples: `InvTest::UseScroll_from_inventory` (requires inventory system + spell data)
- Pattern: Use `SetUpTestSuite()` to load assets once, then test interactions

**Performance Benchmarks:**
- Framework: Google Benchmark
- Scope: Measure performance of critical paths
- Approach: Iterate operation in benchmark loop, report timing
- Examples: `path_benchmark.cpp`, `crawl_benchmark.cpp`, `light_render_benchmark.cpp`
- Pattern: `void BenchmarkName(benchmark::State &state)` with inner loop

**Example Benchmark:**
```cpp
void BenchmarkMap(const Map &map, benchmark::State &state)
{
    const auto [start, dest] = FindStartDest(map);
    const auto posOk = [&map](Point p) { return map[p] != '#'; };
    constexpr size_t MaxPathLength = 25;
    for (auto _ : state) {
        int8_t path[MaxPathLength];
        int result = FindPath([](Point, Point) { return true; },
            posOk, start, dest, path, MaxPathLength);
        benchmark::DoNotOptimize(result);
    }
}
```

## Common Patterns

**Async Testing:**
Not explicitly used; game is single-threaded in test context

**Error Testing:**
```cpp
// Testing optional failures
TEST(DataFileTest, TryLoadMissingFile)
{
    auto result = LoadDataFile("txtdata\\not_found.tsv");
    EXPECT_FALSE(result.has_value())
        << "Trying to load a non-existent file should return an unexpected/error response";
    EXPECT_EQ(result.error(), DataFile::Error::NotFound)
        << "The error code should indicate the file was not found";
}
```

**State Assertion:**
```cpp
// Testing state changes through operations
TEST(PathTest, Heuristics)
{
    constexpr Point source { 25, 32 };
    Point destination = source;
    EXPECT_EQ(TestPathGetHeuristicCost(source, destination), 0)
        << "Wrong cost for travelling to the same tile";
}
```

**Collection Testing:**
```cpp
// Using matchers for collection assertions
TEST(CrawlTest, VisitationOrderTest)
{
    std::vector<Displacement> order;
    Crawl(0, 2, [&](Displacement displacement) {
        order.push_back(displacement);
        return false;
    });
    EXPECT_THAT(order, ElementsAre(
        Displacement(0, 0),
        Displacement(0, 1), Displacement(0, -1),
        Displacement(-1, 0), Displacement(1, 0)
    ));
}
```

**Parameterized Testing:**
```cpp
// Array-based parameterized test data
BlockTestCase BlockData[] = {
    { 6, Warrior, Normal },
    { 7, Rogue, Normal },
    { 8, Sorcerer, Normal },
};

for (const auto &testCase : BlockData) {
    int frames = RunBlockTest(testCase.maxRecoveryFrame, testCase.itemFlags);
    EXPECT_EQ(frames, testCase.expectedRecoveryFrame);
}
```

**Assertion Ordering:**
- Use `ASSERT_*` for critical preconditions (test will stop on failure)
- Use `EXPECT_*` for individual assertions (test continues, collecting all failures)
- Pattern: Load data with `ASSERT_TRUE()`, then verify with `EXPECT_*`

## Test Entry Point

**Location:** `test/main.cpp`

**Responsibilities:**
1. Disable error dialogs for headless testing: `devilution::HeadlessMode = true`
2. Configure platform-specific paths (e.g., macOS app bundle paths)
3. Initialize Google Test framework
4. Register custom test listeners
5. Run all tests

**Example:**
```cpp
int main(int argc, char **argv)
{
    devilution::HeadlessMode = true;
    testing::InitGoogleTest(&argc, argv);

    testing::TestEventListeners &listeners =
        testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new SkippedTestListener());

    return RUN_ALL_TESTS();
}
```

## Test Configuration

**CMake Integration:**
- Tests enabled by default: `option(BUILD_TESTING "Build tests." ON)`
- Test executable: `DevilutionX_test`
- Test discovery: CMake automatically discovers test files
- Fixtures managed via `Fixtures.cmake` for data files

**Build with Tests:**
```bash
cmake -DBUILD_TESTING=ON ..
cmake --build . --target RUN_TESTS
```

---

*Testing analysis: 2026-02-01*
