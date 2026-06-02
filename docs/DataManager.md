# DataManager — Panduan Penggunaan

## Overview

`DataManager` adalah singleton yang menyediakan akses ke `PlayerData.json` secara terpusat.
Semua modul membaca/menulis dari **instance yang sama** (bukan copy).

```
Game
 └─ DataManager::getInstance()
      ├── m_data (json, live in memory)
      ├── load()   — baca dari file
      └── save()   — tulis ke file
           ▲
           │
      ┌────┴────┬──────────┐
      │         │          │
   GameOver  Leaderboard  Menu/Skill
   (write)   (read)       (read)
```

## API

```cpp
// Ambil instance (sama di semua modul)
auto& dm = DataManager::getInstance();

// Load/save
dm.load();                       // default: "./data/PlayerData.json"
dm.load("./path/to/file.json");  // path custom
dm.save();                       // simpan ke file yg terakhir di-load

// Akses data (LIVE REFERENCE, bukan copy)
json& data = dm.getData();
const json& data = dm.getData(); // read-only

// Ganti path
dm.setFilePath("./data/PlayerData.json");
```

## Struktur `PlayerData.json`

```json
{
    "data": [
        {
            "username": "hyperion",
            "highest_score": 10000,
            "research_point": 300,
            "unlocked_word_id": [],
            "unlcoked_skill_id": []
        }
    ]
}
```

## Contoh per Modul

### 1. Game — inisialisasi awal

```cpp
// Game::Game()
DataManager::getInstance().load();
```

### 2. GameOver — simpan score setelah game over

```cpp
#include "DataManager.h"

// GameOver::Update() — saat player simpan score
auto& dm = DataManager::getInstance();
auto& data = dm.getData();

for (auto& p : data["data"]) {
    if (p["username"] == playerName) {
        if (score > p["highest_score"].get<int>())
            p["highest_score"] = score;
        goto saved;
    }
}
data["data"].push_back({
    {"username", playerName},
    {"highest_score", score},
    {"research_point", 0},
    {"unlocked_word_id", json::array()},
    {"unlcoked_skill_id", json::array()}
});
saved:
dm.save();
```

### 3. Leaderboard — baca daftar score

```cpp
#include "DataManager.h"

// LeaderboardSystem::LoadFromJSON()
auto& dm = DataManager::getInstance();
dm.load();  // refresh dari file

players.clear();
for (auto& it : dm.getData()["data"]) {
    PlayerData p;
    p.name    = it.value("username", "?");
    p.score   = it.value("highest_score", 0);
    players.push_back(p);
}
SortPlayers();
```

### 4. Skill system — cek/unlock skill

```cpp
// Cek apakah shield sudah ter-unlock
auto& data = DataManager::getInstance().getData();
auto& skills = data["data"][0]["unlcoked_skill_id"];

bool hasShield = false;
for (auto& id : skills) {
    if (id == "shield") { hasShield = true; break; }
}

// Unlock skill baru
if (!hasShield) {
    skills.push_back("shield");
    DataManager::getInstance().save();
}
```

## Aturan Penting

1. **`getData()` return reference** — perubahan langsung ke memory, jangan copy
2. **`save()` harus dipanggil manual** — perubahan di memory tidak otomatis ke file
3. **Hanya 1 player** — `data["data"][0]` untuk player pertama. Jika mau multi-player, tambah logika lookup by username
4. **Include cukup `"DataManager.h"`** — json otomatis includes

## Checklist Integrasi

| Modul | Perlu include? | Perlu diubah? |
|---|---|---|
| Game | `DataManager.h` | Panggil `load()` di constructor |
| GameOver | `DataManager.h` | Ganti `ScoreManager` → `DataManager` |
| Leaderboard | `DataManager.h` | Ganti `LoadFromJSON` → baca dari `getData()` |
| GameplayManager | `DataManager.h` | Untuk unlock skill / research point |
| MainMenu | `DataManager.h` | Opsional — reset data dll |

## Catatan

- `DataManager` belum diintegrasikan ke modul manapun — implementasi sesuai checklist di atas
- File `data/PlayerData.json` sudah ada dengan data dummy
- Header di `include/DataManager.h`, implementasi di `src/DataManager.cpp`
- `ScoreManager.h` (namespace) masih ada sebagai referensi — bisa dihapus setelah migrasi selesai
