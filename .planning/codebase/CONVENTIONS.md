# Coding Conventions

**Analysis Date:** 2026-02-01

## Naming Patterns

**Files:**
- Header files: `.h` or `.hpp` extension
- Implementation files: `.cpp` extension
- Test files: `*_test.cpp` suffix (e.g., `path_test.cpp`, `random_test.cpp`)
- Benchmark files: `*_benchmark.cpp` suffix (e.g., `path_benchmark.cpp`)
- Snake case with underscores for multi-word files (e.g., `data_file_test.cpp`)

**Functions:**
- Global functions: `PascalCase` (e.g., `FindPath`, `GenerateRandomNumber`, `StartPlrHit`)
- Member functions: `camelCase` (e.g., `SetUp`, `OnTestPartResult`, `GetInstance`)
- Private/internal functions: `camelCase` prefixed with underscore or in anonymous namespace (e.g., `FreeDlg`, `LoadDataFile`)

**Variables:**
- Local variables: `camelCase` (e.g., `result`, `startPosition`, `destinationPosition`)
- Member variables: `camelCase` with underscore suffix (configured via clang-tidy, e.g., `missingMpqAssets_`, `totalSkipped`)
- Global variables: Flexible casing (see clang-tidy config: `GlobalVariableCase: aNy_CasE`)
- Constants: `camelCase` or `PascalCase` (e.g., `MaxPathLengthMonsters`, `PathAxisAlignedStepCost`)

**Types:**
- Classes: `PascalCase` (e.g., `InvTest`, `SkippedTestListener`, `ParseIntResult`)
- Structs: `PascalCase` (e.g., `BlockTestCase`, `Map`, `GameLogicStep`)
- Enums: `PascalCase` for enum types, `PascalCase` for enum constants (e.g., `SelectionRegion`, `CLICK_NONE`, `PM_GOTHIT`)
- Template parameters: `PascalCase` (e.g., `T`, `N` - single letters per clang-tidy config)
- Type aliases: `PascalCase` (e.g., `ParseIntResult`)

## Code Style

**Formatting:**
- Tool: `clang-format` with WebKit style (`BasedOnStyle: webkit`)
- Tab width: 4 spaces (configured as `UseTab: ForIndentation`)
- Line comment alignment: Enabled (`AlignTrailingComments: true`)
- Short blocks on single line allowed (`AllowShortBlocksOnASingleLine: true`)
- Short if statements without braces allowed for single-line bodies (`AllowShortIfStatementsOnASingleLine: WithoutElse`)
- Pointer alignment: Right side (`PointerAlignment: Right`, e.g., `int* p;`)
- Namespace indentation: None (`NamespaceIndentation: None`)
- Fix namespace comments enabled (`FixNamespaceComments: true`)
- Qualifier alignment: Left (`QualifierAlignment: Left`)
- Include sorting: Enabled (`SortIncludes: true`)

**Linting:**
- Tool: `clang-tidy`
- Configuration file: `.clang-tidy` in source root
- Enabled checks: Extensive coverage (bugprone, cppcoreguidelines, google-runtime-int, llvm, misc, modernize, performance, portability, readability)
- Notable disabled checks: `-readability-identifier-length`, `-modernize-avoid-c-arrays`, `-modernize-use-trailing-return-type`
- Usage: Run with `run-clang-tidy -p build 'Source.*'` from project root
- Auto-fix: Run with `run-clang-tidy -p build -fix -format 'Source.*'`

## Import Organization

**Order:**
1. Standard C++ headers (e.g., `<cstdint>`, `<array>`, `<string_view>`)
2. Third-party headers (e.g., `<gtest/gtest.h>`, `<gmock/gmock.h>`, `<benchmark/benchmark.h>`, `<fmt/format.h>`)
3. Project headers using quotes (e.g., `"diablo.h"`, `"engine/path.h"`)

**Path Aliases:**
- Headers use relative paths from project root
- All includes in quotes: `#include "path/to/header.h"`
- No angle brackets for local headers
- Examples: `#include "engine/path.h"`, `#include "utils/paths.h"`, `#include "data/file.hpp"`

**Namespace Organization:**
- Files typically wrap implementation in `namespace devilution { }`
- Test files nest unnamed namespace for local helpers: `namespace devilution { namespace { ... } }`
- Helper functions in anonymous namespace to avoid polluting outer namespace

## Error Handling

**Patterns:**
- Use `std::optional<T>` for operations that may fail (e.g., `LoadDataFile()` returns `std::optional`)
- Use `tl::expected<T, E>` for operations returning value or error (e.g., parsing operations)
- Result types return `has_value()` / `value()` / `error()` pattern
- Fatal errors call `app_fatal()` which displays dialog and exits
- For data loading failures: Check `has_value()` before calling `value()`
- Test assertions check `EXPECT_FALSE(result.has_value())` for error cases
- Validation errors use `expected<void, std::string>` for error messages

**Example Pattern:**
```cpp
auto result = LoadDataFile("path/file.tsv");
if (!result.has_value()) {
    EXPECT_EQ(result.error(), DataFile::Error::NotFound);
    return;
}
const DataFile &dataFile = result.value();
```

## Logging

**Framework:** `fmt::format` for formatting, `std::cout` for console output in some contexts

**Patterns:**
- Format strings: `fmt::format(fmt::runtime(pattern), args)`
- Used for error messages and diagnostic output
- In test context: Custom `SkippedTestListener` tracks skipped tests and reasons
- Test output uses structured logging to stdout for skip summaries

**Example:**
```cpp
fmt::format(fmt::runtime(_("Error: {:s}")), errorMessage)
std::cout << "Test Skip Summary\n";
```

## Comments

**When to Comment:**
- Use `//` style comments in headers for enum values and complex declarations
- Doxygen-style comments (`/** ... */`) for public interfaces and file headers
- `@brief` and `@file` tags document functions and files
- Parameter documentation with `@param` tags for complex functions

**JSDoc/Doxygen:**
- File headers include `@file` and brief description
- Function headers document parameters with `@param` and return with `@return`
- Example: `@file cursor.h` with `@brief Interface of cursor tracking functionality`
- Used throughout to generate documentation

**Example:**
```cpp
/**
 * @file path.h
 *
 * Interface of the path finding algorithms.
 */
```

## Function Design

**Size:**
- No strict line limits; functions range from 5 lines to hundreds
- Inline helpers common for single-use operations
- Complex algorithms kept together for clarity (e.g., `FindPath` implementation)

**Parameters:**
- Pass by const reference for large objects: `const DataFile &dataFile`
- Function callbacks use `tl::function_ref<bool(Point, Point)>` for lightweight non-owning references
- Test helper functions pass objects by reference for mutation: `void SetUp() override`

**Return Values:**
- Single values returned directly: `int FindPath(...)`
- Optional results: `std::optional<T>`
- Results with error info: `tl::expected<T, E>`
- Void for operations that succeed or fail (with error handling elsewhere)

**Const Correctness:**
- Member functions marked `const` when appropriate
- Override implementations use `override` keyword explicitly
- Parameters use `const` for read-only access

## Module Design

**Exports:**
- Headers expose public interface; implementation in `.cpp` files
- extern declarations for functions needed in tests (e.g., `extern bool TestPlayerDoGotHit(Player &player);`)
- Static/internal functions in `.cpp` files or anonymous namespaces

**Barrel Files:**
- Not used; individual headers included explicitly
- Each module has specific headers for its interface
- Tests include specific headers needed

**Header Guards:**
- Use `#pragma once` (supported by all modern compilers used)
- Located at top of header files before includes

## Structured Bindings and Modern C++

**C++ Standard:** C++17+ features used extensively
- Structured bindings: `const auto [start, dest] = FindStartDest(map);`
- std::optional, std::expected for error handling
- Range-based for loops throughout
- Function_ref for lightweight callbacks
- constexpr for compile-time constants and functions

**Example Pattern:**
```cpp
for (const auto &[reason, count] : skipReasons) {
    // Process key-value pairs
}
```

---

*Convention analysis: 2026-02-01*
