# AGENTS.md — TypingShooter (Cosmic Keypad)

## Build & Run

```powershell
mkdir build -ea 0; pushd build; cmake .. -G "MinGW Makefiles"; cmake --build .; popd
build\game.exe
```

No tests, no lint, no formatter, no CI — do not run them.

## Architecture

- **State machine** in `Game.cpp` (`Update<State>()` / `Draw<State>()`) with 11 states (`State.h`).
- **`GameplayManager` → `Game`** communication via `std::function` callbacks (avoids circular deps). Never give `GameplayManager` direct player access.
- **`DataManager`** singleton (`getInstance()`) owns `PlayerData.json` read/write. `ScoreManager` is legacy — use `DataManager`.
- **Window**: 1080×720, `SetExitKey(KEY_F12)`, 60 FPS.

## Known Incomplete (Do Not Assume These Work)

| Issue | Detail |
|---|---|
| **Hardcoded player** | `Game` constructor sets `m_username = "hyperion"` — replace when login flow is finished |
| **Login/Register** | `UpdateLoginRegister()` / `DrawLoginRegister()` stubs exist but are empty |
| **Skill effects** | 3 skills (Instant Crit, Chrono Stasis, Score Booster) defined in TechTree but **not wired to gameplay** |
| **AVL tree** | Not implemented — planned for leaderboard |
| **Word gating** | `WordSystem::getRandomWord()` doesn't filter by unlocked words yet |

## Key Conventions

- **All comments & UI text in Bahasa Indonesia** — code identifiers in English.
- Section markers in `.cpp` files: `// ===== 🔄 SECTION NAME =====`.
- Custom data structures for academic credit: `SinglyLinkedList.h` (template), Circular Doubly Linked List (MainMenu, UnlockedWords), fixed-array stack (ComboStack), graph+BFS (TechTree), manual Selection Sort (UnlockedWords).
- `GameConfig.h` namespace `Config` for all constants (spawn timers, cooldowns, debug flags, etc).
- Vendored deps: `ext/raylib/libraylib.a` (Win), `ext/nlohmann/json.hpp`.

## Deeper References

- `docs/PROGRESS.md` — full feature status, academic checklist, data flow diagram, implementation roadmap
- `docs/COMMENT_RULES.md` — commenting conventions
- `docs/DataManager.md` — DataManager API and integration guide
