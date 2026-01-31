# DevilutionX

DevilutionX is a source port of Diablo and Hellfire, striving to make it simple to run the game on modern platforms while providing engine improvements, bug fixes, and optional quality-of-life features.

## Project Structure

*   **Source/**: Contains the main C++ source code.
    *   `main.cpp`: Application entry point (delegates to `devilution::DiabloMain`). Supports SDL2 and SDL3.
    *   `diablo.cpp`: Core game loop and initialization (`DiabloMain`).
    *   `engine/`: Reusable game engine components.
    *   `control/`: Input handling.
    *   `utils/`: Utility functions.
*   **CMake/**: CMake modules and toolchain files for cross-platform builds.
*   **docs/**: Documentation, including build instructions (`building.md`) and manuals.
*   **Packaging/**: Platform-specific packaging scripts and resources (Windows, macOS, Linux, Switch, etc.).
*   **3rdParty/**: Vendor dependencies.

## Building and Running

The project uses CMake as its build system.

### Prerequisites

*   **CMake**
*   **C++ Compiler** (GCC, Clang, MSVC)
*   **Dependencies**: SDL2, SDL2_image, libsodium, libpng, libfmt, googletest (for tests).
    *   On Windows, these are often managed via `vcpkg` or provided MinGW scripts.
    *   On Linux/macOS, use the system package manager (`apt`, `brew`).

### Build Commands

**Standard Release Build:**

```bash
cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release
cmake --build build -j <num_cores>
```

**Debug Build:**

```bash
cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j <num_cores>
```

**Running the Game:**
After building, the executable (e.g., `devilutionx`) will be in the `build/` directory (or a subdirectory depending on the generator).
*   **Important**: You need `DIABDAT.MPQ` from the original game in the same folder as the executable or in the project root to run the game.

## Development Conventions

*   **Language**: C++ (C++17 standard is typically used).
*   **Formatting**: The project uses `.clang-format` for code style. strictly adhere to it.
*   **Static Analysis**: `.clang-tidy` is used for static analysis.
*   **Testing**: Unit tests use GoogleTest and are located in `test/`.
    *   To build tests: Pass `-DBUILD_TESTING=ON` to CMake.
    *   To run tests: `ctest` in the build directory.

## Key Files

*   `Source/diablo.h`: Main header with global state and function declarations (legacy structure).
*   `Source/init.cpp`: Game initialization logic.
*   `Source/control/`: Handling of mouse and keyboard input.
