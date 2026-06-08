# Aturan Penulisan Komentar

## Bahasa
Semua komentar ditulis dalam **Bahasa Indonesia**.

---

## Header Files (`.h`)

### 1. File
```cpp
// NamaFile.h — deskripsi singkat
```

### 2. Class / Struct / Enum
```cpp
// Deskripsi singkat tentang class ini
class NamaClass {
```

### 3. Method — di atas deklarasi
```cpp
void Update();          // Handle input dan update logika per-frame
void Draw();            // Render ke layar
bool IsActive() const;  // Cek apakah objek sedang aktif
```

### 4. Member variable — inline di samping
```cpp
int m_score;            // Skor akumulasi player
bool m_isActive;        // Status aktif objek
std::string m_name;     // Nama display
```

---

## Source Files (`.cpp`)

### 1. Method sections
```cpp
// ===============================
// 🔄 SECTION NAME
// ===============================
```

### 2. Inside functions — comment BEFORE the logic block
```cpp
// Cari asteroid dengan huruf pertama cocok
auto targets = asteroidManager.scanAllAsteroids([c](const Asteroid& ast) {
    return ast.active && !ast.word.empty() && ast.word[0] == c;
});
```

### 3. Edge cases
```cpp
// Jika list kosong, tampilkan pesan dan return
if (m_nodeCount == 0) {
    ...
    return;
}
```

### 4. Non-obvious algorithms
```cpp
// BFS dari BARRIER untuk menentukan skill mana yang AVAILABLE
// Skill root (BARRIER) selalu di-set AVAILABLE jika masih LOCKED
std::queue<SkillName> q;
q.push(BARRIER);
```

### 5. Complex conditions
```cpp
// Wrap-around: jika indeks melebihi batas, kembali ke 0
m_selectedIndex = (m_selectedIndex + 1) % m_nodeCount;
```

---

## Prinsip Umum

| Situasi | Wajib komentar? |
|---|---|
| Fungsi publik di `.h` | **Ya** — jelaskan apa yang dilakukan |
| Parameter/return tidak jelas | **Ya** |
| Algoritma rumit (BFS, sorting, wrap) | **Ya** — jelaskan cara kerja |
| Edge case (null, empty, overflow) | **Ya** |
| Magic number / konstanta | **Ya** — jelaskan artinya |
| Loop sederhana (for i = 0..n) | Tidak perlu |
| Getter/setter trivial | Tidak perlu |
| `i++`, `break`, `continue` | Tidak perlu |
| `#include` / `#pragma once` | Tidak perlu |

---

## Contoh Lengkap

### Header
```cpp
// PlayerManager.h — mengelola data profil player

struct PlayerData {
    std::string username;       // Nama display player
    int highScore;              // Skor tertinggi yang pernah dicapai
    bool isOnline;              // Status koneksi player
};

class PlayerManager {
public:
    void LoadPlayers();         // Baca data dari file JSON
    void SavePlayers() const;   // Simpan data ke file JSON
    PlayerData* FindPlayer(const std::string& name);  // Cari player berdasarkan username

private:
    std::vector<PlayerData> m_players;   // Koleksi semua player
};
```

### Source
```cpp
void PlayerManager::LoadPlayers() {
    // Buka file JSON dan parse ke struktur
    std::ifstream file("data.json");
    if (!file.is_open()) return;    // Jika file tidak ada, skip

    json data;
    file >> data;
    file.close();

    // Iterasi tiap entry dan konversi ke PlayerData
    for (const auto& entry : data["players"]) {
        PlayerData pd;
        pd.username = entry["username"];
        pd.highScore = entry.value("high_score", 0);
        m_players.push_back(pd);
    }
}
```
