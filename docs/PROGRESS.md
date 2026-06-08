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
| **Main Menu** | Circular doubly linked list (6 opsi) |
| **Leaderboard** | Sort by score, podium animation, glitch transisi |
| **Credit** | Auto-scroll + manual scroll, looping |
| **Skill Tree (SKILL LAB)** | Node rendering, dependency edges, BFS state update, unlock + RP deduction, right-click tooltip dengan word-wrap |
| **Dictionary (WORD DICTIONARY)** | Semua 150 kata, search realtime, definisi word-wrap, scroll, circular navigasi |
| **Word Bank** | Hanya kata ter-unlock, circular doubly linked list backend, search realtime |
| **Word Unlock + RP** | Setiap kata berhasil diketik full → +10 RP + simpan ke PlayerData.json |
| **Data Persistence** | `DataManager` singleton, baca/tulis `PlayerData.json` |
| **Player Info Debug** | Panel overlay (username, score, RP, unlocked words/skills) |
| **Background** | Scrolling dual-texture seamless |
| **Audio** | Musik lobby looping, musik credit, SFX laser/error/glitch |

### 🔶 Sebagian

| Fitur | Status | Masalah |
|---|---|---|
| **Login/Register** | State enum + method stub sudah ada (`LOGIN_AND_REGISTER`, `LOGOUT`) | `UpdateLoginRegister()` dan `DrawLoginRegister()` masih kosong. Player masih hardcoded ke `"hyperion"` |
| **Skill — Instant Crit** | Skill data + node di tree sudah ada | Belum diintegrasikan ke gameplay (`Asteroid::typingAsteroid()` belum pakai pengecekan skill) |
| **Skill — Chrono Stasis** | Skill data + node di tree sudah ada | Belum diintegrasikan ke gameplay (belum perlambat asteroid) |
| **Skill — Score Booster** | Skill data + node di tree sudah ada | Belum diintegrasikan ke scoring system |

### ❌ Belum

| Fitur | Catatan |
|---|---|
| **Register flow** | Belum ada form input username |
| **Logout flow** | Belum ada implementasi |
| **Skill effects di gameplay** | Skill yang di-unlock di SKILL LAB belum mempengaruhi mekanik game |

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

## Implementasi Wajib — Status & Lokasi

Berdasarkan daftar implementasi yang harus ada di projek UAS:

### Algoritma

| # | Item | Status | Lokasi |
|---|---|---|---|
| 1 | **struct** | ✅ | `DataManager.h:8` (PlayerProfile), `Asteroid.h:5` (Asteroid), `TechTree.h:25` (SkillData), `Dictionary.h:5` (DictionaryEntry), `UnlockedWords.h:5` (WordNode), `MainMenu.h:6` (MenuNode) |
| 2 | **references** | ✅ | `TechTreeUI.h:7` (TechTree& di constructor), callback params `const std::string&`, `GameplayManager.h:17` (std::function) |
| 3 | **pointer** | ✅ | `GameplayManager.h:29` (Asteroid* currentTarget), `Game.h:33` (GameplayManager*), linked list nodes (WordNode*, MenuNode*) |
| 4 | **namespace** | ✅ | `GameConfig.h:5` (namespace Config), `Asteroid.cpp:11` (anonymous namespace untuk helper) |
| 5 | **callback function** | ✅ | `GameplayManager.h:33-34` (ScoreCallback, AsteroidDestroyedCallback), dipasang di `Game.cpp:58-67` via `std::function` + lambda |
| 6 | **default argument** | ✅ | `Asteroid.h:26` — `asteroidType(const int tier = 1)` |
| 7 | **function overloading / template** | ✅ | `GameplayManager.h:48-49` — `AddScore(int)` dan `AddScore(int, int)` overload; `SinglyLinkedList.h` — template class |
| 8 | **exception handling** | ❌ | Belum ada try-catch di seluruh codebase |
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
| 20 | **binary tree / avl tree** | ❌ | Belum diimplementasikan |
| 21 | **graph dengan BFS / DFS** | ✅ | `TechTree.cpp:111-128` — BFS dari BARRIER untuk menentukan `uiState` (LOCKED/AVAILABLE/UNLOCKED) tiap skill |
| 22 | **hashing & hash table** | ✅ | `word_module.h` — `std::unordered_map` untuk definisi kata (easy/medium/hard_definitions); `TechTree.h:39` — `std::unordered_map<SkillName, SkillData>` |
| 23 | **sorting manual** | ❌ | Belum ada implementasi sorting manual (Leaderboard masih pakai `std::sort` atau mekanisme bawaan) |

### Keterangan Status
- ✅ = sudah diimplementasikan dan berfungsi
- ❌ = belum diimplementasikan

---

## Rencana Implementasi

Berdasarkan item yang masih ❌ dan fitur yang 🔶 Sebagian, direncanakan sebagai berikut:

### Prioritas 1 — Melengkapi Item Wajib

#### 1. Exception Handling — `DataManager`
**Masalah:** `DataManager::load()` dan `save()` tidak punya proteksi. File corrupt/crash diam-diam.
**Rencana:**
- `load()`: try-catch `std::ifstream` gagal → buka → buat data default. JSON parse error → reset ke array kosong.
- `save()`: try-catch `std::ofstream` gagal → fallback console error.
- `TechTree::SkillNameFromString()`: return default atau throw exception untuk string tidak dikenal.
- File: `src/DataManager.cpp`, `src/TechTree.cpp`

#### 2. AVL Tree — Leaderboard
**Masalah:** Leaderboard sorted pakai array + `std::sort` setiap kali, bukan struktur data murni.
**Rencana:**
- Class `AVLTree` baru: key = score, value = username.
- Insert otomatis balance dengan rotasi kiri/kanan.
- In-order traversal untuk ranking descending.
- Integrasi ke `LeaderboardSystem` untuk menyimpan & mengambil data terurut.
- File baru: `include/AVLTree.h`, `src/AVLTree.cpp`

#### 3. Sorting Manual — Word Bank
**Masalah:** Sorting Dictionary pake `std::sort`, belum ada implementasi sorting algoritma manual.
**Rencana:**
- Di **Word Bank**, tambahkan opsi sort manual (Selection Sort atau Bubble Sort).
- Tekan tombol `S` untuk sort A-Z, `D` untuk sort Z-A.
- Buat method `SortManual()` yang mengubah urutan node di Circular Doubly Linked List tanpa STL.
- File: `src/UnlockedWords.cpp`, `include/UnlockedWords.h`

### Prioritas 2 — Integrasi Skill ke Gameplay

| Skill | Rencana Implementasi |
|---|---|
| **Instant Crit** | Di `Asteroid::typingAsteroid()`, jika player punya skill ini dan kata masih penuh (first huruf cocok) → langsung `active = false` tanpa perlu ketik sisa huruf. |
| **Chrono Stasis** | Saat skill aktif, kurangi `velocity.x` dan `velocity.y` semua asteroid aktif menjadi 50% untuk durasi tertentu. Timer di `GameplayManager` atau `SkillManager` baru. |
| **Score Booster** | Di `AddScore()`, kalikan base score dengan multiplier tambahan jika skill aktif. Tidak stack dengan combo. |

### Prioritas 3 — Login / Register Flow

- Implementasi `UpdateLoginRegister()` dan `DrawLoginRegister()` dengan form input username.
- `CreatePlayer()` sudah ada di DataManager — tinggal panggil dari flow register.
- `FindPlayer()` sudah ada — panggil dari flow login.
- Ganti hardcoded `"hyperion"` dengan player hasil login.

### Prioritas 4 — Word Gating di Gameplay

- `WordSystem::getRandomWord()` saat ini ambil dari pool besar tanpa filter.
- Rencana: tambah parameter `const std::vector<std::string>& unlockedWords`, filter pool berdasarkan kata yang sudah di-unlock.
- Atau: jika `unlocked_words` kosong, fallback ke beberapa kata default agar pemain baru tetap bisa main.

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
