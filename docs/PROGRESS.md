# Progress & Gambaran Terkini

## Architektur

```
Game (state machine)
├── MENU
│   └── MainMenu (circular doubly linked list)
│       ├── Play           → GAMEPLAY
│       ├── Leaderboard    → LEADERBOARD
│       ├── Skills         → UNLOCK_SKILL (SKILL LAB)
│       ├── Word Bank      → UNLOCKED_WORDS
│       ├── Credit         → CREDIT
│       ├── Logout         → LOGOUT
│       └── Quit           → exit
├── GAMEPLAY
│   └── GameplayManager
│       ├── AsteroidManager (pool[50] + shower SinglyLinkedList)
│       ├── Spaceship (laser targeting)
│       ├── ComboStack (fixed array, max 6 level)
│       ├── ShieldSkill (cooldown 30s)
│       └── BombSkill (cooldown 30s, shockwave animasi)
├── PAUSE
├── GAME_OVER
├── LEADERBOARD
├── CREDIT
├── UNLOCK_SKILL (SKILL LAB)
│   ├── TechTree (backend: dependency graph, BFS state)
│   └── TechTreeUI (frontend: node rendering, tooltip)
├── WORD_DICTIONARY (F1 dari Main Menu)
│   └── Dictionary (search realtime, word-wrap)
└── UNLOCKED_WORDS (WORD BANK)
    └── UnlockedWords (circular DLL, search realtime)
```

---

## Status Fitur

### ✅ Selesai & Berfungsi

| Fitur | Detail |
|---|---|
| **Gameplay** | Asteroid spawn, typing untuk hancurkan, scoring, combo multiplier |
| **Asteroid Pool** | 50 asteroid fixed pool, spawn bergiliran |
| **Asteroid Shower** | SinglyLinkedList untuk event hujan asteroid, priority queue (shower > normal) |
| **Difficulty** | 6 tier (Easy/Medium/Hard × 2 kecepatan) |
| **Combo Stack** | Fixed-size array (max 6), Push setiap 5 kata, Pop saat salah ketik |
| **Shield Skill** | Cooldown 30s, tahan 1 hantaman |
| **Bomb Skill** | Cooldown 30s, shockwave radius 1100, animasi alpha fade |
| **Main Menu** | Circular doubly linked list (7 opsi) |
| **Leaderboard** | AVL Tree (insert descending + inorder traversal), podium animation, glitch transisi |
| **Credit** | Auto-scroll + manual scroll, looping, reset scroll tiap masuk |
| **Skill Tree (SKILL LAB)** | Node rendering, dependency edges, BFS state update, unlock + RP deduction, right-click tooltip dengan word-wrap |
| **Dictionary (WORD DICTIONARY)** | Semua 150 kata, search realtime, definisi word-wrap, scroll, circular navigasi |
| **Word Bank** | Hanya kata ter-unlock, circular doubly linked list backend, search realtime, manual selection sort A-Z / Z-A |
| **Word Unlock + RP** | Setiap kata berhasil diketik full → +10 RP + simpan ke PlayerData.json |
| **Data Persistence** | `DataManager` singleton, baca/tulis `PlayerData.json` |
| **Player Info Debug** | Panel overlay (username, score, RP, unlocked words/skills) |
| **Background** | Scrolling dual-texture seamless |
| **Audio** | Musik lobby looping, musik credit, bgm default untuk state tanpa musik, SFX laser/error/glitch/explosion |
| **Exception Handling** | Try-catch di `DataManager` (parse error, file write, type error) + `TechTree` (throw invalid_argument) |
| **Login Screen** | Form input username, validasi ke DataManager, ENTER submit, TAB ke register, ESC quit |
| **Register Screen** | Form input username, cek unique, CreatePlayer(), ESC back ke login |
| **Logout Screen** | Timer 1 detik, tampilkan pesan, transisi otomatis ke login |
| **Glitch Transition** | Overlay visual (tidak blocking), 0.6 detik, glitch scanlines, SFX masuk/keluar |
| **BGM Default** | `UpdateDefault()` auto-start bgm.mp3 untuk state tanpa musik spesifik |
| **Aura Field Skill** | 10s duration, 30s cooldown, lingkaran ungu, absorbs all hits |
| **Instant Crit Skill** | 10s duration, 30s cooldown, kotak merah, first letter instant destroy |
| **Score Booster Skill** | 5s duration, 30s cooldown, multiplier 16x, lingkaran emas |
| **Active Skill System** | Keybind `1`–`N` sequential (berdasarkan unlock order), HUD top-right hanya skill ter-unlock, state machine IDLE→ACTIVE→COOLDOWN |
| **Explosion Particles** | Pool 64 explosions, max 35 particles per explosion, alpha fade, random direction/speed |

### ✅ Sudah Tidak Digunakan / Dihapus

| Fitur | Alasan |
|---|---|
| **Chrono Stasis** | Dihapus dari TechTree agar BARRIER memiliki tepat 3 children (AURA, SCORE_BOOSTER, SHOCKWAVE) |

### ❌ Tidak Ada

Semua fitur yang direncanakan sudah diimplementasikan dan berfungsi.

---

## Data Flow

```
PlayerData.json
      ↕ (load/save)
DataManager (singleton)
      ↕
PlayerProfile (struct)
      ↕
Game.m_currentPlayer
      ├── TechTree (baca RP & unlocked_skills)
      ├── TechTreeUI (tampilkan state)
      ├── Dictionary (abaikan profile — semua kata)
      ├── UnlockedWords (hanya unlocked_words)
      ├── GameOver (update highest_score)
      └── GameplayManager (callback → unlock word + RP)
```

---

## Circular Dependencies & Catatan Arsitektur

- `GameplayManager` tidak punya akses langsung ke `m_currentPlayer` — komunikasi via callback `std::function<void(const std::string&)>` yang dipasang di `Game` (constructor + restartGame).
- `DataManager` adalah singleton — semua akses via `DataManager::getInstance()`.
- `TechTreeUI` menerima referensi `TechTree` di constructor — UI dan backend terpisah.
- `UnlockedWords` menggunakan Circular Doubly Linked List sebagai struktur data utama (backend), dengan `std::vector<WordNode*>` sebagai view untuk search.
- `MainMenu` juga menggunakan Circular Doubly Linked List untuk navigasi.
- `AsteroidManager` menggunakan SinglyLinkedList untuk event asteroid shower.

---

## Riwayat Penyelesaian

Semua item implementasi wajib sudah selesai:

### Algoritma & Exception Handling ✅

| # | Item | Status | Lokasi |
|---|---|---|---|
| 1 | **struct** | ✅ | `DataManager.h:8` (PlayerProfile), `Asteroid.h:5` (Asteroid), `TechTree.h:25` (SkillData), `Dictionary.h:5` (DictionaryEntry), `UnlockedWords.h:5` (WordNode), `MainMenu.h:6` (MenuNode) |
| 2 | **references** | ✅ | `TechTreeUI.h:7` (TechTree& di constructor), callback params `const std::string&`, `GameplayManager.h:17` (std::function) |
| 3 | **pointer** | ✅ | `GameplayManager.h:29` (Asteroid* currentTarget), `Game.h:33` (GameplayManager*), linked list nodes (WordNode*, MenuNode*) |
| 4 | **namespace** | ✅ | `GameConfig.h:5` (namespace Config), `Asteroid.cpp:11` (anonymous namespace untuk helper) |
| 5 | **callback function** | ✅ | `GameplayManager.h:33-34` (ScoreCallback, AsteroidDestroyedCallback), dipasang di `Game.cpp:58-67` via `std::function` + lambda |
| 6 | **default argument** | ✅ | `Asteroid.h:26` — `asteroidType(const int tier = 1)` |
| 7 | **function overloading / template** | ✅ | `GameplayManager.h:48-49` — `AddScore(int)` dan `AddScore(int, int)` overload; `SinglyLinkedList.h` — template class |
| 8 | **exception handling** | ✅ | `DataManager.cpp` — try-catch untuk parse_error, file write, type_error; `TechTree.cpp` — throw std::invalid_argument + catch di loadFromProfile |
| 9 | **STL — vector / list** | ✅ | `std::vector` — `DataManager.h:12-13`, `Dictionary.h:18-19`, `AsteroidManager.h:62` (priority_queue), dll |
| 10 | **STL — iterator** | ✅ | `Dictionary.cpp:37-38` (loop with begin/end), `Dictionary.cpp:130-131` (iterator di draw loop) |
| 11 | **STL — sort** | ✅ | `Dictionary.cpp:32-35` (std::sort pada m_entries) |
| 12 | **STL — find** | ✅ | `Dictionary.cpp:79-82` (std::find pada m_wordStrings), `UnlockedWords.cpp:55-68` (std::find pada word pools) |
| 13 | **STL — count** | ✅ | `Dictionary.cpp:44-46` (std::count per difficulty) |
| 14 | **file handling** | ✅ | `DataManager.cpp:7-29` (ifstream/ofstream untuk baca/tulis `PlayerData.json`) |
| 15 | **lambda function** | ✅ | `Game.cpp:59-66` (callback unlock word), `Dictionary.cpp:33-34` (sort comparator), `GameplayManager.cpp:93-96` (scan predicate) |

### Struktur Data

| # | Item | Status | Lokasi |
|---|---|---|---|
| 16 | **linked list (single)** | ✅ | `SinglyLinkedList.h` — template class untuk `AsteroidManager::asteroidShower` |
| 17 | **circular linked list** | ✅ | `MainMenu.cpp` — Circular Doubly Linked List untuk navigasi menu; `UnlockedWords.cpp` — Circular Doubly Linked List untuk daftar kata |
| 18 | **stack** | ✅ | `ComboStack.h` / `ComboStack.cpp` — fixed-size array stack (max 6 level, tiap Push = ×2 multiplier) |
| 19 | **queue** | ✅ | `AsteroidManager.h:62` — `std::priority_queue<EventType>` (ASTEROID_SHOWER prioritas > NORMAL) |
| 20 | **binary tree / avl tree** | ✅ | `Leaderboard.h:26-58` / `Leaderboard.cpp:39-112` — AVL Tree dengan rotate kiri/kanan, balance factor, insert descending by score, inorder traversal |
| 21 | **graph dengan BFS / DFS** | ✅ | `TechTree.cpp:111-128` — BFS dari BARRIER untuk menentukan `uiState` (LOCKED/AVAILABLE/UNLOCKED) tiap skill |
| 22 | **hashing & hash table** | ✅ | `word_module.h` — `std::unordered_map` untuk definisi kata; `TechTree.h:39` — `std::unordered_map<SkillName, SkillData>` |
| 23 | **sorting manual** | ✅ | `UnlockedWords.cpp` — Selection Sort O(n²) manual, tekan S/D untuk A-Z / Z-A |

### Fitur Game

| Fitur | Detail |
|---|---|
| **Login/Register/Logout** | Form input keyboard, validasi DataManager, create/find player, timer logout |
| **Aura Field Skill** | 10s duration, 30s cooldown, absorbs all hits, visual lingkaran ungu |
| **Instant Crit Skill** | 10s duration, 30s cooldown, first letter instant destroy, visual kotak merah |
| **Score Booster Skill** | 5s duration, 30s cooldown, multiplier 16x, visual lingkaran emas |
| **Active Skill System** | Keybind `1`–`N` sequential, HUD top-right, state machine IDLE→ACTIVE→COOLDOWN |
| **Explosion Particles** | Pool 64 explosions, max 35 particles, alpha fade, random direction |
| **Glitch Transition** | Overlay visual non-blocking, 0.6s, SFX masuk/keluar |
| **BGM Default** | `UpdateDefault()` auto-start bgm.mp3 untuk state tanpa musik |

### Catatan

- Chrono Stasis dihapus dari TechTree (keputusan desain: BARRIER memiliki tepat 3 children)
- Semua komentar dalam Bahasa Indonesia sesuai `docs/COMMENT_RULES.md`

---

## Konstanta Penting (`GameConfig.h`)

| Konstanta | Value | Keterangan |
|---|---|---|
| `screenWidth` | 1080 | Lebar window |
| `screenHeight` | 720 | Tinggi window |
| `showerEventInterval` | 30s | Interval antar shower event |
| `normalSpawnInterval` | 2.0s | Interval spawn asteroid biasa |
| `shieldCooldown` | 30s | Cooldown skill shield |
| `bombCooldown` | 30s | Cooldown skill bomb |
| `bombShockwaveMaxRadius` | 1100 | Radius maksimal shockwave bomb |
| `enableDebugPlayerInfo` | true | Tampilkan overlay debug player |
