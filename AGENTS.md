# AGENTS.md — TypingDefense-Strukdata-Algoritma

## Build & Run

```bash
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
./game.exe
```

- Build auto-copies `assets/` and `data/` into the build output directory.
- No test framework or test files exist in this repo.
- On Linux, raylib must be system-installed; Windows bundles `ext/raylib/libraylib.a`.

## Project structure

| Path | Role |
|---|---|
| `src/main.cpp` | Entrypoint — creates `Game` and calls `Run()` |
| `include/` | All headers (`.h`) |
| `src/` | All source files (`.cpp`) |
| `ext/` | Vendored dependencies (raylib, nlohmann/json) |
| `data/` | Runtime persistence: `data.json` (leaderboard), `PlayerData.json` |
| `build/compile_commands.json` | Present for editor tooling |

## Architecture notes

- **State machine** in `Game` — states: MENU, GAMEPLAY, PAUSE, GAME_OVER, LEADERBOARD, CREDITS.
- **DataManager** singleton (`include/DataManager.h`) — reads/writes `PlayerData.json`.
- **ScoreManager** namespace (`include/ScoreManager.h`) — legacy, should be replaced by DataManager.
- **SinglyLinkedList** (`include/SinglyLinkedList.h`) — custom template linked list, an academic requirement. Used in `AsteroidManager` for shower event management.
- **ComboStack** (`include/ComboStack.h`) — fixed-size array stack (max 6 levels), custom DS.
- **Asteroid pool** — fixed array of 50 `Asteroid` structs (`poolAsteroid[50]`).
- **Event priority queue** — `std::priority_queue<EventType>` where `ASTEROID_SHOWER` (2) > `NORMAL` (1).
- **Skill cooldowns** — Shield and Bomb both use 30s cooldown.
- **Word pool** — 150 words in `word_module.h`, split into Easy (1-5), Medium (6-10), Hard (11+ letters).

## Naming & conventions

- Some member variables use `m_` prefix.
- Game constants in `GameConfig.h` under `namespace Config`.
