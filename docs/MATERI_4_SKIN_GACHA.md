# Materi Presentasi 4 — Skin Shop + Gacha

## 📋 Ringkasan

Bagian ini membahas **Skin Shop** (pembelian dan pemilihan skin spaceship) dan **Gacha Crate** (sistem undian untuk mendapatkan skin dan reward). Struktur data utama: **STL Vector** untuk menyimpan skin yang sudah di-unlock, dan **weighted random selection** untuk gacha.

---

## 1. Skin System

### Data Skin

**File:** `include/SkinData.h`

Ada **15 skin spaceship** dalam grid **5 kolom × 3 baris** di spritesheet `Spaceships.png`:

| ID | Nama | Rarity | Harga (RP) | Kolom | Baris |
|----|------|--------|------------|-------|-------|
| 0 | Green Arrow | Common (1) | 0 (free) | 0 | 0 |
| 1 | Silver Fang | Uncommon (2) | 150 | 1 | 0 |
| 2 | Jade Serpent | Rare (3) | 300 | 2 | 0 |
| 3 | Solar Storm | Epic (4) | 500 | 3 | 0 |
| 4 | Void Sovereign | Legendary (5) | 800 | 4 | 0 |
| 5 | Red Phantom | Common (1) | 0 (free) | 0 | 1 |
| 6 | Rust Reaver | Uncommon (2) | 150 | 1 | 1 |
| 7 | Crimson Wraith | Rare (3) | 300 | 2 | 1 |
| 8 | Inferno Lord | Epic (4) | 500 | 3 | 1 |
| 9 | Obsidian King | Legendary (5) | 800 | 4 | 1 |
| 10 | Blue Stryker | Common (1) | 0 (free) | 0 | 2 |
| 11 | Frost Hunter | Uncommon (2) | 150 | 1 | 2 |
| 12 | Arctic Phantom | Rare (3) | 300 | 2 | 2 |
| 13 | Nova Empress | Epic (4) | 500 | 3 | 2 |
| 14 | Starlight Emperor | Legendary (5) | 800 | 4 | 2 |

**Rarity system:**
| Rarity | Label | Warna |
|--------|-------|-------|
| 1 | COMMON | Hijau |
| 2 | UNCOMMON | Abu-abu terang |
| 3 | RARE | Biru langit |
| 4 | EPIC | Ungu |
| 5 | LEGENDARY | Oranye |

### SkinManager

**File:** `src/SkinManager.cpp`, `include/SkinManager.h`

Mengelola data skin pemain:

| Method | Kegunaan |
|--------|----------|
| `init(username)` | Load data skin player dari JSON |
| `save()` | Simpan data skin ke JSON |
| `isUnlocked(skinId)` | Cek apakah skin sudah dimiliki |
| `unlockSkin(skinId)` | Tambah skin ke koleksi |
| `getActiveSkin()` | Ambil ID skin yang sedang dipakai |
| `setActiveSkin(skinId)` | Ganti skin aktif |
| `getUnlockedSkins()` | Daftar semua skin yang dimiliki (vector) |
| `getRP()` / `addRP()` / `spendRP()` | Manajemen Research Points |

**Data per player di JSON:**
```json
{
    "username": "hyperion",
    "research_point": 420,
    "free_spin": 2,
    "active_skin": 7,
    "unlocked_skins": [0, 5, 10, 1, 6]
}
```

### STL Vector untuk Unlocked Skins

```cpp
vector<int> unlockedSkins;  // Menyimpan ID skin yang sudah di-unlock
```

**Operasi yang digunakan:**
```cpp
// Cek apakah skin sudah di-unlock
bool isUnlocked(int skinId) const {
    return find(unlockedSkins.begin(), unlockedSkins.end(), skinId) != unlockedSkins.end();
}

// Tambah skin ke koleksi
void unlockSkin(int skinId) {
    if (!isUnlocked(skinId)) unlockedSkins.push_back(skinId);
}
```

**STL yang digunakan:**
| STL | Kegunaan | File:Baris |
|-----|----------|-----------|
| `vector<int>` | Menyimpan daftar skin ID | SkinManager.h:45 |
| `find()` | Mencari skin dalam vector | SkinManager.cpp:128 |
| `push_back()` | Menambah skin baru | SkinManager.cpp:133 |

---

## 2. Skin Select Screen

**File:** `src/SkinSelect.cpp`, `include/SkinSelect.h`

### Tampilan

```
┌──────────────────────────────────────────────────────────────┐
│                     SKIN BAY                                 │
│  Arrow: Navigate | ENTER: Equip/Buy | G: Gacha | ESC: Back  │
│                                                    RP: 420   │
│                                                    Skins: 5/15│
│                                                              │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌─────┐ │
│  │ Green    │ │ Silver   │ │ Jade     │ │ Solar    │ │Void │ │
│  │ Arrow    │ │ Fang     │ │ Serpent  │ │ Storm    │ │Sov. │ │
│  │ EQUIPPED │ │          │ │          │ │          │ │     │ │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘ └─────┘ │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌─────┐ │
│  │ Red      │ │ Rust     │ │Crimson   │ │ Inferno  │ │Obs. │ │
│  │ Phantom  │ │ Reaver   │ │ Wraith   │ │ Lord     │ │King │ │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘ └─────┘ │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌─────┐ │
│  │ Blue     │ │ Frost    │ │ Arctic   │ │ Nova     │ │Star.│ │
│  │ Stryker  │ │ Hunter   │ │ Phantom  │ │ Empress  │ │Empr  │ │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘ └─────┘ │
│                                                              │
│  Void Sovereign  |  LEGENDARY  |  EQUIPPED                   │
└──────────────────────────────────────────────────────────────┘
```

### Navigasi

| Tombol | Aksi |
|--------|------|
| Arrow Keys (← → ↑ ↓) | Navigasi grid skin |
| ENTER | Equip (jika sudah dimiliki) / Buy (jika belum) |
| G | Buka Gacha Crate |
| ESC | Kembali ke menu |

### Mekanisme Beli Skin

```
Player pilih skin yang belum dimiliki
  ↓
Tekan ENTER
  ↓
Cek: RP cukup? (mgr.spendRP(info.price))
  ↓
Jika cukup → skin di-unlock, langsung di-equip, RP berkurang
Jika tidak → tidak terjadi apa-apa (gagal beli)
```

---

## 3. Gacha Crate System

**File:** `src/GachaScreen.cpp`, `include/GachaScreen.h`

### Tampilan

```
┌──────────────────────────────────────────────────────────────┐
│                   RESEARCH CRATE                             │
│                        RP: 420                               │
│                     Free Spins: 1                            │
│                                                              │
│           ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐           │
│           │ZONK  │  │+10RP │  │ZONK  │  │+25RP │           │
│           └──────┘  └──────┘  └──────┘  └──────┘  ...      │
│           ◄── carousel berputar ──►                          │
│                                                              │
│        [ENTER] Open Crate (100 RP)     [ESC] Back            │
│                     Skins: 5/15                              │
└──────────────────────────────────────────────────────────────┘
```

### Phase Gacha

```
 IDLE ──(ENTER)──→ SPINNING ──(selesai)──→ LANDED ──(timer)──→ RESULT ──(ENTER)──→ IDLE
                     │                        │                    │
                 Carousel                   Tampilkan           Tampilkan
                 berputar                   highlight           full screen
                                            di tengah           result
```

### Pool Gacha (Carousel)

Ada **24 item** dalam carousel pool:

| Tipe | Jumlah | Rarity | Contoh |
|------|--------|--------|--------|
| ZONK | 4 | 0 | ZONK_1, ZONK_2, ZONK_3, ZONK_4 |
| COIN | 4 | 1-3 | +10 RP, +25 RP, +50 RP, +100 RP |
| FREE_SPIN | 1 | 3 | Free spin |
| SKIN | 15 | 1-5 | Semua 15 skin |

### Reward Probability

```
1-500  (50%)   → ZONK
501-590 (9%)   → COIN +10
591-700 (11%)  → COIN +25
701-790 (9%)   → COIN +50
791-850 (6%)   → COIN +100
851-900 (5%)   → FREE_SPIN
901-1000 (10%) → SKIN

Jika skin, rarity dibagi lagi:
  1-50   (5% overall)   → Common skin
  51-80  (3% overall)   → Uncommon
  81-93  (1.3% overall) → Rare
  94-98  (0.5% overall) → Epic
  99-100 (0.2% overall) → Legendary
```

### Spin Animation

Saat player menekan ENTER:
1. Tentukan reward (SelectReward)
2. Hitung target posisi di carousel
3. Animasi spin selama 3-4 detik dengan **easing cubic**: `1 - (1-t)^3`
4. Setelah berhenti, highlight item yang menang
5. Tampilkan result screen

### Weighted Random Selection

**File:** `SkinManager.cpp:148-164`

Untuk gacha skin (bukan reward tetap), ada sistem **weighted random**:

```cpp
int SkinManager::weightedRandomSelect(const vector<int>& ids) const {
    // Bobot berdasarkan kolom (rarity)
    int weightsByCol[5] = {0, 45, 30, 17, 8};
    // Col 0 = common (bobot 0 = tidak bisa dari gacha)
    // Col 1 = uncommon (bobot 45)
    // Col 2 = rare (bobot 30)
    // Col 3 = epic (bobot 17)
    // Col 4 = legendary (bobot 8)
    
    int totalWeight = 0;
    for (int id : ids) totalWeight += weightsByCol[skinToCol(id)];
    
    int roll = GetRandomValue(1, totalWeight);
    int cumulative = 0;
    for (int id : ids) {
        cumulative += weightsByCol[skinToCol(id)];
        if (roll <= cumulative) return id;  // Terpilih!
    }
    return ids.back();
}
```

**Bobot per kolom (rarity):**

| Kolom | Rarity | Bobot di Gacha |
|-------|--------|----------------|
| 0 | Common | 0 (tidak dapat dari gacha) |
| 1 | Uncommon | 45 |
| 2 | Rare | 30 |
| 3 | Epic | 17 |
| 4 | Legendary | 8 |

### Free Spin

Player bisa mendapat **Free Spin** dari hasil gacha atau dari data JSON:
- `getFreeSpinCount()` — lihat jumlah free spin
- `useFreeSpin()` — gunakan 1 free spin (tidak bayar RP)
- Jika ada free spin, tulisan berubah: "[ENTER] Use Free Spin" bukan bayar 100 RP

### All Collected

Jika semua 15 skin sudah dimiliki, gacha menampilkan pesan "ALL SKINS COLLECTED!" dan tidak bisa spin lagi.

---

## 4. Implementasi Struktur Data

### STL Vector

| Lokasi | Kegunaan |
|--------|----------|
| `SkinManager::unlockedSkins` | Menyimpan ID skin yang sudah di-unlock |
| `GachaScreen::pool` | Menyimpan 24 item carousel |

### STL Algorithm: find()

```cpp
// Cari skin di vector
bool isUnlocked(int skinId) const {
    return find(unlockedSkins.begin(), unlockedSkins.end(), skinId) != unlockedSkins.end();
}
```

### Lambda Function

**File:** `GachaScreen.cpp:35-37`

```cpp
auto addItem = [&](GachaItemType type, int id, int rarity, const char* name, Color color) {
    pool.push_back({ type, id, rarity, name, color });
};
```

Lambda ini dipakai untuk menambahkan item ke pool gacha dengan lebih ringkas.

---

## 5. File yang Relevan

| File | Baris | Kegunaan |
|------|-------|----------|
| `src/SkinManager.cpp` | 176 | Manajemen skin + RP + gacha pull |
| `include/SkinManager.h` | 54 | Header skin manager |
| `include/SkinData.h` | 102 | Data 15 skin, enum rarity, helper functions |
| `src/SkinSelect.cpp` | 208 | UI grid skin shop |
| `include/SkinSelect.h` | 29 | Header skin select |
| `src/GachaScreen.cpp` | 587 | Sistem gacha lengkap |
| `include/GachaScreen.h` | 74 | Header gacha screen |
| `src/DataManager.cpp` | 127 | Load/save data player (termasuk skin) |

---

## 6. Jawaban untuk Tanya Jawab

| Pertanyaan | Jawaban |
|------------|---------|
| Berapa jumlah skin? | 15 skin dalam grid 5×3 |
| Struktur data untuk unlocked skins? | `vector<int>` (STL Vector) |
| Gimana cara dapet skin baru? | Beli di Skin Shop (RP) atau dapat dari Gacha |
| Berapa harga skin termahal? | 800 RP (Legendary) |
| Berapa harga skin gratis? | ID 0, 5, 10 gratis (Common) |
| Gimana cara gacha bekerja? | Roll random 1-1000, weighted probability |
| Berapa persen dapet legendary dari gacha? | 0.2% (langka banget) |
| Apa itu free spin? | Gacha gratis tanpa bayar RP |
| Gimana carousel berputar? | Animasi dengan easing cubic, 3-4 detik |
| STL apa aja yang dipakai? | `vector`, `find()` |
