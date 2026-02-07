# External Integrations

**Analysis Date:** 2026-02-01

## APIs & External Services

**Discord Rich Presence:**
- Discord - In-game status and rich presence notifications
  - SDK/Client: Discord SDK at `3rdParty/discord/`
  - Integration: `Source/discord/discord.h` and `Source/discord/discord.cpp`
  - Enabled via: `DISCORD_INTEGRATION=ON` CMake option
  - Usage: Game state updates via `discord_manager::UpdateGame()` and `discord_manager::UpdateMenu()`

## Data Storage

**File Format Support:**
- MPQ (Blizzard archive format) - Game asset containers
  - Client: libmpq at `3rdParty/libmpq/`
  - Conditional: Enabled when `SUPPORTS_MPQ=ON`
  - Tools: smpq executable (external) for building MPQ archives

**Save Files:**
- Local filesystem - `.sv` (standard saves) and `.hsv` (hardcore saves)
- Optional unpacked format: `UNPACKED_SAVES=ON` CMake option uses loose files instead of MPQ format

**Asset Archives:**
- Smacker video codec support via `3rdParty/libsmackerdec`

**File Access:**
- No database integration - purely file-based persistence
- Core MPQ operations at `Source/mpq/` directory

## Authentication & Identity

**Auth Provider:**
- Custom networking layer - No centralized auth service
- Multiplayer sessions use peer-to-peer or ZeroTier VPN authentication

**Steam Integration:**
- Steam game detection (Windows only)
  - Implementation: `3rdParty/find_steam_game/`
  - Purpose: Auto-locate Steam-installed Diablo game data
  - Conditional: Windows only, found via `find_steam_game` CMake module

## Networking

**Multiplayer Modes:**
1. **TCP Direct Connection:**
   - Protocol: Custom frame-based protocol over TCP
   - Location: `Source/dvlnet/tcp_client.h`, `Source/dvlnet/tcp_server.h`
   - ASIO-based async I/O: `3rdParty/asio/`
   - Use case: Direct IP multiplayer

2. **ZeroTier VPN:**
   - Provider: ZeroTier One (opensource VPN)
   - SDK/Client: libzt at `3rdParty/libzt/`
   - Implementations:
     - Native mode: `Source/dvlnet/zerotier_native.h`
     - LWIP mode: `Source/dvlnet/zerotier_lwip.h`
   - Protocol wrapper: `Source/dvlnet/protocol_zt.h`
   - Use case: NAT traversal and cross-platform multiplayer

**Packet Encryption:**
- Optional encryption via libsodium
  - Client: libsodium at `3rdParty/libsodium/`
  - Enabled via: `PACKET_ENCRYPTION=ON` CMake option
  - Key type: `crypto_secretbox_KEYBYTES` (32 bytes)
  - Location: `Source/dvlnet/packet.h` defines encryption structures

**Network Abstraction:**
- Abstract interface: `Source/dvlnet/abstract_net.h`
- Base protocol: `Source/dvlnet/base_protocol.h`
- Implementations:
  - TCP client/server: `Source/dvlnet/tcp_client.h`, `Source/dvlnet/tcp_server.h`
  - Loopback (single-player): `Source/dvlnet/loopback.h`
  - ZeroTier variants: `Source/dvlnet/zerotier_*.h`

**Game Protocol:**
- Frame-based packet protocol at `Source/dvlnet/packet.h`
- Frame queue for reliable delivery at `Source/dvlnet/frame_queue.h`
- Turn-based synchronization for game state

**Multiplayer UI:**
- Menu system at `Source/DiabloUI/multi/`

## Monitoring & Observability

**Error Tracking:**
- None - Errors are handled locally and logged

**Logs:**
- Console output via `fmt` library
- Utility logging at `Source/utils/log.hpp`
- No remote logging integration

**Debug Support:**
- Clang-tidy static analysis (`.clang-tidy` at `Source/.clang-tidy`)
- AddressSanitizer (ASAN) support via `-DASAN=ON`
- UndefinedBehaviorSanitizer (UBSAN) support via `-DUBSAN=ON`
- ThreadSanitizer (TSAN) support via `-DTSAN=ON`
- GPerfTools profiler support via `-DGPERF=ON`

## Accessibility Integration

**Screen Reader Support:**
- **Windows:** Tolk library at `3rdParty/tolk/`
- **Unix-like:** Speechd (Speech Dispatcher) integration
  - Enabled via: `SCREEN_READER_INTEGRATION=ON`
  - Header finder: `CMake/finders/FindSpeechd.cmake`

**Text Direction:**
- SheenBidi library at `3rdParty/SheenBidi/`
  - Purpose: Bidirectional text rendering for RTL languages
  - Version: 2.9.0+

## CI/CD & Deployment

**Hosting:**
- GitHub - Source repository and releases at https://github.com/diasurgical/devilutionX

**CI Pipeline:**
- GitHub Actions workflow (`.github/workflows/`)
- Builds across 15+ platforms (Windows, Linux, macOS, Android, iOS, Nintendo Switch, PS4, Xbox, Vita, 3DS, Amiga)
- Build artifacts available as GitHub Actions workflow artifacts

**Packaging:**
- CPack integration for installers:
  - Windows: NSIS installer (optional) or ZIP archive
  - Linux: DEB packages (Debian/Ubuntu), RPM packages (RedHat/Fedora), 7Z archive
  - macOS: DMG disk image

**Code Quality:**
- Codecov integration for coverage reporting (badge in README)
- `.editorconfig` for consistent code formatting at `Source/.editorconfig`
- `.clang-format` at `Source/.clang-format` for C++ formatting
- `.clang-tidy` at `Source/.clang-tidy` for linting rules

## Environment Configuration

**Required env vars:**
- None explicitly required at build time
- CMake options handle all configuration

**Secrets location:**
- No secrets management - open-source project
- Steam game location is auto-detected or must be specified to CMake

**Config Files:**
- `CMakeLists.txt` - Main build configuration
- `CMakeSettings.json` - Visual Studio CMake integration (Windows)
- `vcpkg.json` - Dependency manifest with conditional features
- `CMake/Dependencies.cmake` - Dependency resolution logic
- `CMake/Platforms.cmake` - Platform-specific overrides

**Feature Flags (CMake):**
```
USE_SDL1               # Use SDL1.2 instead of SDL2
USE_SDL3               # Use SDL3 instead of SDL2
NONET                  # Disable all networking
PACKET_ENCRYPTION      # Enable libsodium encryption
DISCORD_INTEGRATION    # Enable Discord rich presence
SCREEN_READER_INTEGRATION # Enable screen reader support
DISABLE_TCP            # Disable TCP multiplayer
DISABLE_ZERO_TIER      # Disable ZeroTier multiplayer
BUILD_TESTING          # Build test targets
UNPACKED_MPQS          # Use unpacked asset folders instead of MPQ
UNPACKED_SAVES         # Use loose save files instead of MPQ saves
DISABLE_STREAMING_MUSIC # Disable streaming music playback
DISABLE_STREAMING_SOUNDS # Disable streaming sound playback
```

## Webhooks & Callbacks

**Incoming:**
- None - Game is standalone, no server backend

**Outgoing:**
- Discord Rich Presence updates (if `DISCORD_INTEGRATION=ON`)
- GitHub Actions CI/CD pipeline triggers on push/PR

---

*Integration audit: 2026-02-01*
