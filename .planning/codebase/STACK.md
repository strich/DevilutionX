# Technology Stack

**Analysis Date:** 2026-02-01

## Languages

**Primary:**
- C++ 20 - Core game engine and server implementation
- C - Selected legacy components and platform-specific code

**Secondary:**
- Lua 5.4 - Modding and scripting support via `sol2` bindings

## Runtime

**Environment:**
- Cross-platform (Windows, Linux, macOS, Android, iOS, Nintendo Switch, PS4, Xbox, PlayStation Vita, Nintendo 3DS, Amiga)
- No specific runtime requirement; compiles to native executables

**Package Manager:**
- vcpkg - Manages C++ dependencies
- Lockfile: `vcpkg.json` present with version `1.6.0`

## Frameworks

**Core:**
- SDL2 (default) or SDL3 (optional) 2.0+ - Cross-platform graphics, audio, and input
- CMake 3.22+ - Build system for all platforms
- Threads (pthread on Unix-like systems)

**Networking:**
- ASIO - Asynchronous I/O for TCP connections via `3rdParty/asio`
- libzt (ZeroTier) - VPN-based multiplayer networking via `3rdParty/libzt`
- Custom dvlnet layer - Abstract networking interface at `Source/dvlnet/`

**Audio/Graphics:**
- SDL_image - Image loading and format support
- SDL_mixer or SDL3_mixer - Audio mixing and playback
- SDL_audiolib - Audio library for SDL2-based audio
- libsmackerdec - Smacker video codec support via `3rdParty/libsmackerdec`

**Scripting/Modding:**
- Lua 5.4 - Embedded Lua VM at `3rdParty/Lua`
- sol2 - C++/Lua bindings at `3rdParty/sol2`

**Testing:**
- GTest (Google Test) - Unit testing framework via `3rdParty/googletest`
- Benchmark - Performance benchmarking via `3rdParty/benchmark`

**Build/Dev:**
- ccache - Compiler cache for faster rebuilds
- clang-tidy - Static analysis (`.clang-tidy` configuration at `Source/.clang-tidy`)

## Key Dependencies

**Critical:**
- fmt 8.0.0+ - String formatting library via `3rdParty/libfmt`
- magic_enum 0.9.7+ - Compile-time enum reflection via `3rdParty/magic_enum`
- unordered_dense - High-performance hash map at `3rdParty/unordered_dense`
- SheenBidi 2.9.0+ - Bidirectional text support at `3rdParty/SheenBidi`

**Infrastructure:**
- libsodium - Cryptographic library for packet encryption (optional, conditional on `PACKET_ENCRYPTION`)
- libmpq - MPQ file format support (used when `SUPPORTS_MPQ=ON`) at `3rdParty/libmpq`
- PKWare - Compression utilities at `3rdParty/PKWare`
- zlib - Compression library at `3rdParty/zlib`
- bzip2 - Compression library (MPQ dependency) at `3rdParty/bzip2`

**Optional Integrations:**
- Discord SDK - Rich presence support at `3rdParty/discord` (enabled via `DISCORD_INTEGRATION=ON`)
- Tolk - Screen reader integration at `3rdParty/tolk` (Windows) or Speechd (Unix-like, enabled via `SCREEN_READER_INTEGRATION=ON`)
- libpng - PNG image format support at `3rdParty/libpng`
- PicoSHA2 - SHA2 hashing at `3rdParty/PicoSHA2`
- tl - Utility template library at `3rdParty/tl`

**Platform-Specific:**
- find_steam_game - Steam game detection on Windows at `3rdParty/find_steam_game`
- Custom platform headers for 3DS, Switch at `Source/platform/{ctr,switch}/`

## Configuration

**Build Options:**
- `CMAKE_CXX_STANDARD`: 20 (C++20 required)
- `CMAKE_CXX_EXTENSIONS`: ON (Use gnu++20 extensions on POSIX)
- `CMAKE_EXPORT_COMPILE_COMMANDS`: ON (clang-tidy support)

**Conditional Build Features:**
- `USE_SDL1`: Use SDL1.2 instead of SDL2 (OFF by default)
- `USE_SDL3`: Use SDL3 instead of SDL2 (OFF by default)
- `NONET`: Disable network support (OFF by default)
- `PACKET_ENCRYPTION`: Enable libsodium packet encryption (ON by default if networking enabled)
- `DISCORD_INTEGRATION`: Build with Discord SDK (OFF by default, set to ON in CMakeSettings.json)
- `SCREEN_READER_INTEGRATION`: Build with screen reader support (OFF by default)
- `BUILD_TESTING`: Build test targets (ON by default)
- `DISABLE_TCP`: Disable TCP multiplayer (OFF by default)
- `DISABLE_ZERO_TIER`: Disable ZeroTier multiplayer (OFF by default)

**Dependency Options:**
- `DEVILUTIONX_SYSTEM_*`: Use system-installed dependencies (most ON by default)
- `DEVILUTIONX_STATIC_*`: Link dependencies statically (most ON by default)
- `BUILD_ASSETS_MPQ`: Package assets into MPQ format (auto-detected based on smpq availability)

**Optimization Options:**
- `DISABLE_LTO`: Disable link-time optimization (OFF by default, enabled in Release)
- `PIE`: Position-independent code (OFF by default, ON when `BUILD_TESTING=ON`)
- `DEVILUTIONX_PROFILE_GENERATE`: Build with PGO profiling (OFF by default)
- `DEVILUTIONX_PROFILE_USE`: Use PGO profile data (OFF by default)

**Debug/Profiling Options:**
- `ASAN`: AddressSanitizer (OFF by default)
- `UBSAN`: UndefinedBehaviorSanitizer (OFF by default)
- `TSAN`: ThreadSanitizer (OFF by default, incompatible with ASAN)
- `DEBUG`: Debug mode in engine (OFF by default)
- `GPERF`: Build with GPerfTools profiler (OFF by default)

## Platform Requirements

**Development:**
- CMake 3.22 or higher
- C++20 compatible compiler (GCC, Clang, MSVC)
- Python (for build scripts and translation compilation)
- vcpkg (for dependency management)

**Production:**
- **Windows:** x86_64, x86, or ARM64
- **Linux:** x86_64, AArch64, x86
- **macOS:** x86_64, ARM64 (Apple Silicon)
- **Android:** ARM, ARM64, x86, x86_64
- **iOS:** ARM64
- **Nintendo Switch:** ARM64 (custom toolchain)
- **PlayStation Vita:** ARM Cortex-A9 (custom toolchain)
- **PlayStation 4:** x86_64 (custom toolchain)
- **Xbox One/Series:** Custom toolchain
- **Original Xbox:** NXDK toolchain
- **Nintendo 3DS:** ARM11 (custom toolchain)
- **Amiga:** M68K (custom toolchain)
- **DOS (DJGPP):** 32-bit x86

## Build Artifacts

**Main Executable:**
- Windows: `devilutionx.exe` (or `libdevilutionx` shared on Android)
- Linux/macOS: `devilutionx`
- Nintendo 3DS: `devilutionx.elf` (converted to `.3dsx` and `.cia`)
- DOS: `devx.exe`

**Asset File:**
- `devilutionx.mpq` - Game assets (if `BUILD_ASSETS_MPQ=ON` and smpq is available)

---

*Stack analysis: 2026-02-01*
