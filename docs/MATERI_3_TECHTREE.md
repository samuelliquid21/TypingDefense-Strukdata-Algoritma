# Materi Presentasi 3 — Tech Tree (Skill Lab)

## 📋 Ringkasan

Bagian ini membahas **Tech Tree / Skill Lab** — sistem unlock skill menggunakan Research Points (RP). Struktur data utama: **Graph + BFS** untuk dependency skill, dan **Hash Table (unordered_map)** untuk menyimpan data skill.

---

## 1. Tech Tree Overview

**File:** `src/TechTree.cpp`, `include/TechTree.h`

Tech Tree adalah sistem unlock skill berbentuk **pohon dependensi (Graph)**. Player membeli skill dengan Research Points (RP). Skill tertentu hanya bisa dibuka jika parent-nya sudah di-unlock.

### Diagram Skill Tree

```
                      ┌──────────────────────────┐
                      │        BARRIER            │
                      │   (Root — 30 RP)          │
                      │   "Shield 1 hantaman"     │
                      └────────┬────────┬─────────┘
                               │        │
              ┌────────────────┤        ├──────────────────┐
              ↓                ↓        ↓                  ↓
   ┌──────────┴──────┐  ┌─────┴───────────┐    ┌──────────┴──────────┐
   │   AURA FIELD    │  │  SCORE BOOSTER   │    │     SHOCKWAVE       │
   │   (100 RP)      │  │  (250 RP)        │    │  (300 RP)           │
   │ 10s auto-shield │  │ 16x score 5s     │    │ Hancurkan asteroid  │
   └─────────────────┘  └──────────────────┘    └──────────┬──────────┘
                                                            │
                                                            ↓
                                                   ┌────────┴──────────┐
                                                   │   INSTANT CRIT    │
                                                   │   (500 RP)        │
                                                   │ 1 huruf=hancurkan │
                                                   └───────────────────┘
```

### 5 Skill di Tech Tree

| Skill | Nama Key | RP Cost | Parent(s) | Fungsi |
|-------|----------|---------|-----------|--------|
| Barrier | `barrier` | 30 | (root) | Shield 1 hantaman |
| Aura Field | `aura_field` | 100 | Barrier | Shield otomatis 10 detik |
| Score Booster | `score_booster` | 250 | Barrier | 16× score multiplier 5 detik |
| Shockwave | `shockwave` | 300 | Barrier | Hancurkan semua asteroid |
| Instant Crit | `instant_crit` | 500 | Shockwave | 1 huruf = hancurkan asteroid |

---

## 2. Graph + BFS untuk Dependency Skill

### Representasi Graph

**File:** `TechTree.h:42-43`

```cpp
unordered_map<SkillName, SkillData> skills;                       // Semua node skill
unordered_map<SkillName, vector<SkillName>> adjList;              // Edge parent → child
```

Graph diimplementasikan dengan:
- **Adjacency List** — `unordered_map<SkillName, vector<SkillName>>`
- Setiap parent punya daftar child-nya
- Edge: parent → child (parent harus di-unlock sebelum child bisa AVAILABLE)

### Penambahan Dependency (Edge)

```cpp
void TechTree::addDependency(SkillName parent, SkillName child) {
    adjList[parent].push_back(child);  // Tambah edge parent → child
}
```

Dependency tree dibangun di constructor:
```cpp
TechTree::TechTree() {
    addDependency(BARRIER, AURA_FIELD);
    addDependency(BARRIER, SCORE_BOOSTER);
    addDependency(BARRIER, SHOCKWAVE);
    addDependency(SHOCKWAVE, INSTANT_CRIT);
}
```

### Skill State

Setiap skill punya 3 kemungkinan state:

| State | Arti | Warna di UI |
|-------|------|-------------|
| `LOCKED` | Belum bisa diakses (parent belum di-unlock) | Abu-abu gelap (tidak ditampilkan) |
| `AVAILABLE` | Bisa dibeli (parent sudah di-unlock) | Biru terang |
| `UNLOCKED` | Sudah dibeli dan aktif | Hijau |

### BFS untuk Update State

**File:** `TechTree.cpp:123-163`

Setiap kali ada perubahan (unlock skill, load profile), state semua skill dihitung ulang algoritma **BFS (Breadth-First Search)**:

```cpp
void TechTree::updateSkillStates() {
    SetBaseStates();         // Step 1: UNLOCKED jika isUnlocked, else LOCKED
    BfsUpdateStates();       // Step 2: BFS dari BARRIER untuk set AVAILABLE
    skills[BARRIER].uiState = AVAILABLE;  // Step 3: Root selalu AVAILABLE
}

void TechTree::BfsUpdateStates() {
    queue<SkillName> q;
    unordered_map<SkillName, bool> visited;
    
    q.push(BARRIER);         // Mulai dari root
    visited[BARRIER] = true;
    
    while (!q.empty()) {
        SkillName current = q.front(); q.pop();
        
        if (skills[current].isUnlocked) {
            for (SkillName child : adjList[current]) {
                if (visited[child]) continue;     // Skip yang sudah dikunjungi
                visited[child] = true;
                
                if (!skills[child].isUnlocked) {
                    skills[child].uiState = AVAILABLE;  // Child bisa dibeli
                } else {
                    q.push(child);  // Child sudah di-unlock, lanjut BFS
                }
            }
        }
    }
}
```

**Cara kerja BFS:**
1. Mulai dari node BARRIER (root)
2. Untuk setiap node yang **sudah di-unlock**, child-nya di-set AVAILABLE
3. Jika child juga sudah di-unlock, lanjutkan BFS ke child tersebut
4. Jika child masih LOCKED, berhenti (tidak turun lagi)
5. Gunakan `visited` untuk mencegah node dikunjungi dua kali

**Kompleksitas:** O(V + E) — semua node dan edge dikunjungi tepat satu kali.

### Flow Unlock Skill

```
Player klik node AVAILABLE
  ↓
TechTree::handleInput() — deteksi klik mouse pada bounds node
  ↓
TechTree::unlockSkill() — cek: 
  1. Skill AVAILABLE?
  2. RP cukup? (via SkinManager::spendRP)
  ↓
Jika ya:
  - skill.isUnlocked = true
  - Simpan ke profile (unlocked_skills)
  - updateSkillStates() → BFS update state child
  ↓
Jika tidak: return false (gagal, mungkin RP tidak cukup)
```

---

## 3. Hash Table (unordered_map)

Tech Tree menggunakan **2 unordered_map** (implementasi Hash Table C++ STL):

```cpp
unordered_map<SkillName, SkillData> skills;       // Key: SkillName → Data skill
unordered_map<SkillName, vector<SkillName>> adjList; // Key: parent → List child
```

**Mengapa pake unordered_map?**
- Pencarian data skill berdasarkan nama: **O(1)** rata-rata
- Tidak perlu iterasi linear untuk cari skill
- Key adalah enum `SkillName`, hash built-in dari C++

### Skill Data

```cpp
struct SkillData {
    SkillName name;          // Identitas skill
    int rpCost;              // Biaya RP
    string desc;             // Deskripsi singkat
    bool isUnlocked;         // Status dari save file
    
    Vector2 position;        // Posisi node di UI
    Rectangle bounds;        // Hitbox click/hover
    SkillState uiState;      // LOCKED / AVAILABLE / UNLOCKED
};
```

### Konversi String ↔ Enum

Untuk serialisasi JSON (save/load), ada konversi nama skill:
```cpp
// String (dari JSON) → enum
SkillName SkillNameFromString(const string& str) {
    if (str == "shockwave") return SHOCKWAVE;
    // ... throw exception jika tidak dikenal
}

// Enum → string (ke JSON)
string StringFromSkillName(SkillName name) {
    switch (name) {
        case SHOCKWAVE: return "shockwave";
        // ...
    }
}
```

---

## 4. Research Points (RP) System

**File:** `src/SkinManager.cpp`, `include/SkinManager.h`

### Cara Mendapatkan RP

| Cara | Jumlah | Kapan |
|------|--------|-------|
| Start awal | 300 RP | Pertama kali main |
| Score → RP | score / 100 | Setiap game over |
| Unlock word | +10 RP | Setiap kata baru selesai diketik |
| Gacha COIN | +10 / +25 / +50 / +100 RP | Dari gacha crate |

### Cara Menggunakan RP

| Pengeluaran | Biaya |
|-------------|-------|
| Unlock skill di Tech Tree | 30-500 RP |
| Beli skin di Skin Shop | 150-800 RP |
| Buka gacha crate | 100 RP |

### Manajemen RP (SkinManager)

```cpp
int getRP() const;              // Lihat saldo RP
void addRP(int amount);         // Tambah RP (dari score, unlock word, gacha)
bool spendRP(int amount);       // Kurangi RP (untuk unlock/beli)
```

RP disimpan di `PlayerData.json` per user dan di-load saat login.

---

## 5. Tech Tree UI

**File:** `src/TechTreeUI.cpp`, `include/TechTreeUI.h`

### Tampilan

```
┌──────────────────────────────────────────────────────────────┐
│                     SKILL LAB                                │
│                                        Research Points: 420  │
│                                                              │
│         ┌──────────────┐                                     │
│         │ Aura Field   │              ┌──────────────┐       │
│         │ RP: 100      │              │  Shockwave   │       │
│         └──────┬───────┘              │  RP: 300     │       │
│                │                      └──────┬───────┘       │
│                │                             │               │
│         ┌──────┴───────┐              ┌──────┴───────┐       │
│         │   BARRIER    │              │Instant Crit  │       │
│         │   RP: 30     │              │  RP: 500     │       │
│         └──────┬───────┘              └──────────────┘       │
│                │                                              │
│         ┌──────┴───────┐                                     │
│         │Score Booster │                                     │
│         │  RP: 250     │                                     │
│         └──────────────┘                                     │
│                                                              │
│  Klik kiri = unlock  |  Klik kanan = info  |  ESC = kembali  │
└──────────────────────────────────────────────────────────────┘
```

### Fitur UI

1. **Node skill** — kotak dengan nama + biaya RP
2. **Garis edge** — menghubungkan parent ke child
3. **Warna node**:
   - Biru terang = AVAILABLE (bisa dibeli)
   - Hijau = UNLOCKED (sudah aktif)
   - Abu-abu = LOCKED (tidak ditampilkan)
4. **Hover** — border node jadi putih terang
5. **Tooltip** (klik kanan) — panel informasi detail:
   - Nama skill
   - Deskripsi
   - Biaya RP
   - Status (LOCKED/AVAILABLE/UNLOCKED)
6. **Research Points counter** — pojok kanan atas

### Input

| Tombol | Aksi |
|--------|------|
| Klik kiri pada node AVAILABLE | Unlock skill |
| Klik kanan pada node | Toggle tooltip info |
| ESC | Kembali ke menu |

---

## 6. Implementasi Algoritma & Struktur Data

### Graph + BFS

| Aspek | Detail |
|-------|--------|
| Struktur data | Adjacency List (`unordered_map<SkillName, vector<SkillName>>`) |
| Algoritma | BFS (Breadth-First Search) dari node root BARRIER |
| Kegunaan | Menentukan state AVAILABLE/LOCKED setiap skill |
| Kompleksitas | O(V + E) |
| File | `TechTree.cpp:143-163` |

### Hash Table (unordered_map)

| Aspek | Detail |
|-------|--------|
| Struktur data | `unordered_map` C++ STL |
| Kegunaan 1 | Menyimpan data skill (key: SkillName) |
| Kegunaan 2 | Menyimpan adjacency list (key: parent) |
| Kompleksitas | O(1) rata-rata untuk insert/find |
| File | `TechTree.h:42-43` |

### Exception Handling

```cpp
SkillName TechTree::SkillNameFromString(const string& str) {
    if (str == "shockwave") return SHOCKWAVE;
    // ...
    throw invalid_argument("TechTree: unknown skill name '" + str + "'");
}
```

Saat load profile, exception ditangkap dan skill yang tidak dikenal di-skip:
```cpp
try {
    SkillName name = SkillNameFromString(skillName);
    skills[name].isUnlocked = true;
} catch (const invalid_argument& e) {
    TraceLog(LOG_WARNING, "TechTree: skip unknown skill '%s'", skillName.c_str());
}
```

### STL Vector untuk Child List

```cpp
unordered_map<SkillName, vector<SkillName>> adjList;
```

`vector<SkillName>` menyimpan daftar child untuk setiap parent. Jumlah child tidak tetap, vector bisa dinamis menyesuaikan.

---

## 7. File yang Relevan

| File | Baris | Kegunaan |
|------|-------|----------|
| `src/TechTree.cpp` | 168 | Graph + BFS, unlock logic, load/save |
| `include/TechTree.h` | 77 | enum SkillName, struct SkillData, class TechTree |
| `src/TechTreeUI.cpp` | 243 | Render UI tech tree, tooltip, hover |
| `include/TechTreeUI.h` | 40 | Header TechTreeUI |
| `src/SkinManager.cpp` | 176 | Manajemen RP (add/spend/get) |
| `include/SkinManager.h` | 54 | Header skin manager |
| `include/DataManager.h` | 40 | PlayerProfile struct (unlocked_skills) |

---

## 8. Jawaban untuk Tanya Jawab

| Pertanyaan | Jawaban |
|------------|---------|
| Struktur data apa yang dipakai Tech Tree? | Graph (adjacency list) + Hash Table (unordered_map) |
| Algoritma apa untuk update state skill? | BFS (Breadth-First Search) dari root |
| Berapa skill yang tersedia? | 5 skill |
| Skill apa yang jadi root? | Barrier (30 RP) |
| Berapa total RP untuk unlock semua? | 30 + 100 + 250 + 300 + 500 = 1.180 RP |
| Gimana cara dapat RP? | Score/100, unlock word (+10), gacha coin, starter 300 |
| Kenapa pake unordered_map? | Pencarian O(1), lebih cepat dari vector/array |
| Apa itu adjacency list? | Struktur data graph dimana setiap node punya daftar node tetangganya |
| Gimana cara BFS bekerja? | Mulai dari root, kunjungi child, lanjut jika child sudah di-unlock |
