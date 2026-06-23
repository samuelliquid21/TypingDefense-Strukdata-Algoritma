# Materi Presentasi 2 — Gameplay

## 📋 Ringkasan

Bagian ini membahas inti permainan: bagaimana player mengetik untuk menghancurkan asteroid, sistem scoring dan combo, asteroid yang datang semakin sulit, spaceship, skill aktif, dan efek ledakan.

---

## 1. Game State Machine Typing

**File:** `src/GameplayManager.cpp`, `include/GameplayManager.h`

Gameplay menggunakan **state machine dengan 2 state**:

```
STATE: SEARCH_FOR_TARGET
  └─ Player menekan huruf → cari asteroid dengan huruf pertama cocok
  └─ Jika ketemu → lock target, pindah ke TARGET_LOCKED
  └─ Jika tidak ada → error sound, combo turun

STATE: TARGET_LOCKED  
  └─ Player mengetik huruf berikutnya pada asteroid yang sama
  └─ Jika kata habis → asteroid hancur, combo naik, kembali SEARCH
  └─ Jika salah → error sound, combo turun
  └─ Jika target mati/hilang → reset ke SEARCH
```

### Alur Typing Step-by-Step

```
Player tekan 'A'
  ↓
findNearestTarget('A') — scan semua asteroid, cari yang huruf pertama 'A'
  ↓  
Jika ketemu → lock target → activateLaser() → typingAsteroid('A')
  ↓
typingAsteroid() hapus huruf pertama dari word
  ↓
return 1 (berhasil)
  ↓
AddScore(1) — skor +1 × combo × score booster
  ↓
Cek: apakah instant crit aktif? Jika ya → langsung hancurkan asteroid
  ↓
Jika tidak → state jadi TARGET_LOCKED, tunggu huruf berikutnya
  ↓
Player tekan 'S' → typingAsteroid('S') → hapus 'S' dari word
  ↓
Word kosong? Ya → asteroid hancur, wordsCompleted++, cek combo push
  ↓
Kembali ke SEARCH_FOR_TARGET
```

### Fungsi-fungsi Kunci

| Fungsi | File:Baris | Kegunaan |
|--------|-----------|----------|
| `processAllCharInputs()` | GameplayManager.cpp:184 | Loop semua input per frame |
| `findNearestTarget(c)` | GameplayManager.cpp:206 | Cari asteroid terdekat dengan huruf pertama c |
| `processSearchForTarget(c)` | GameplayManager.cpp:231 | Proses state SEARCH |
| `processTargetTyping(c)` | GameplayManager.cpp:272 | Proses state TARGET_LOCKED |
| `handleSuccessfulTyping(r)` | GameplayManager.cpp:287 | Handle input benar |
| `handleInstantCritOrLock()` | GameplayManager.cpp:251 | Cek instant crit atau lock target |

---

## 2. Asteroid

**File:** `src/Asteroid.cpp`, `include/Asteroid.h`

### Struktur Asteroid

```cpp
struct Asteroid {
    Vector2 position;          // Posisi di layar
    Vector2 velocity;          // Kecepatan gerak
    std::string word;          // Sisa kata yang belum diketik
    std::string originalWord;  // Kata asli (untuk callback scoring)
    int radius;                // Ukuran lingkaran
    int textureId;             // Variasi visual (0-3)
    bool active;               // Aktif/tidak
    bool targeted;             // Sedang ditarget?
};
```

### Cara Asteroid Muncul

1. **Pool (50 slot)** — asteroid normal spawn setiap ~2 detik
2. **Shower (SinglyLinkedList)** — asteroid hujan muncul setiap ~30 detik, bertambah banyak seiring waktu
3. **Priority Queue** — event disimpan dengan prioritas (SHOWER > NORMAL), dieksekusi periodik

Setiap asteroid punya **tier (1-6)** yang menentukan:
- **Kesulitan kata** — Easy (tier 1-2), Medium (tier 3-4), Hard (tier 5-6)
- **Kecepatan** — tier ganjil lambat (50 px/s), tier genap cepat (80 px/s)

### Gerakan Asteroid

- Muncul dari **tepi kiri** layar
- 50% chance: **menuju player** (mengarah ke posisi player)
- 50% chance: **random** (ke atas/bawah)
- Radius proporsional dengan panjang kata: `20 + panjang_kata × 2`

### Fungsi typingAsteroid()

```cpp
int Asteroid::typingAsteroid(char characterTyped) {
    if (!active || word.empty()) return 0;        // Asteroid mati
    if (characterTyped != word[0]) return 0;       // Huruf tidak cocok
    
    word.erase(0, 1);                              // Hapus huruf pertama
    
    if (word.empty()) {                            // Kata habis
        active = false;                            // Hancur
        if (onDestroyed) onDestroyed(position);    // Panggil callback
    }
    
    return 1; // Selalu return 1 per karakter benar
}
```

### Rendering Asteroid

- Lingkaran abu-abu (`DrawCircleV`)
- Sisa kata di tengah lingkaran
- Jika ditarget: kotak merah semi-transparan + label "TARGETED"

---

## 3. Spaceship & Laser

**File:** `src/SpaceShip.cpp`, `include/SpaceShip.h`

- Posisi tetap di **tepi kanan** layar: `(1000, 360)`
- Menggunakan **spritesheet** 15 skin (5×3 grid)
- Saat player mengetik huruf benar → **laser** garis biru dari ship ke asteroid
- Rotasi 180° (menghadap ke kiri)
- Laser aktif hanya ~0.1 detik lalu mati otomatis

```cpp
void SpaceShip::activateLaser(Vector2 &target) {
    laserTarget = target;
    laserIsActive = true;
    laserNextStop = GetTime() + LASER_DELAY;
}
```

---

## 4. Sistem Scoring & Combo

### Perhitungan Skor

```
score += 1 (per karakter) × comboMultiplier × scoreBoosterMultiplier

comboMultiplier: 1× → 2× → 4× → 8× → 16× → 32× → 64× (max 6 level)
scoreBoosterMultiplier: 16× jika skill aktif, 1× jika tidak
```

### Combo Stack (Fixed Array)

**File:** `src/ComboStack.cpp`, `include/ComboStack.h`

Implementasi **Stack menggunakan fixed array (max 6 level)**:

```cpp
class ComboStack {
    int stack[MAX_COMBO_LEVEL];  // Array tetap ukuran 6
    int top;                      // Index puncak stack
};
```

| Operasi | Kegunaan | Kapan Dipanggil |
|---------|----------|-----------------|
| `Push()` | Naikkan combo (×2 dari sebelumnya) | Setiap 5 words selesai diketik |
| `Pop()` | Turunkan combo (hapus level teratas) | Setiap salah ketik |
| `GetMultiplier()` | Ambil multiplier saat ini | Setiap kali AddScore() |

**Progresi combo:**
- 0 kata selesai → 1×
- 5 kata selesai → 2×
- 10 kata selesai → 4×
- 15 kata selesai → 8×
- 20 kata selesai → 16×
- 25 kata selesai → 32×
- 30 kata selesai → 64× (maximum)

### Fungsi AddScore

Ada 2 overload:

```cpp
// Overload 1: dengan combo + score booster
void AddScore(int points) {
    int mult = scoreBoosterSkill.isActive() ? 16 : 1;
    score += points * comboStack.GetMultiplier() * mult;
}

// Overload 2: dengan multiplier kustom + callback
void AddScore(int basePoints, int multiplier) {
    score += basePoints * multiplier;
    if (onScoreChanged != nullptr) onScoreChanged(score, multiplier);
}
```

### HUD Score & Combo

Di tengah atas layar:
- **"Score: 1234"** — putih
- **"4× COMBO"** — kuning jika >1×, abu-abu jika 1×
- **"2/5 words"** — hijau jika >1×, abu-abu jika 1× (progress bar)

---

## 5. Skill Aktif

**File:** `include/Skill.h` (base class)

Ada **5 skill aktif**, masing-masing dengan **3 state**:

```
IDLE → (aktifasi) → ACTIVE → (durasi habis) → COOLDOWN → (timer selesai) → IDLE
```

### Binding Skill ke Tombol

```cpp
void GameplayManager::rebuildActiveSkills() {
    // Scan semua skill yang ter-unlock
    // Urutan: Barrier, Aura Field, Shockwave, Instant Crit, Score Booster
    // Mapping otomatis ke KEY_1, KEY_2, KEY_3, ...
}
```

Skill yang sudah di-unlock muncul di **HUD pojok kanan atas** dengan status:
- **Hijau** — READY [1] (bisa dipakai)
- **Biru** — ACTIVE (sedang berlangsung)
- **Abu-abu** — cooldown countdown

### 5.1 Shield / Barrier

**File:** `src/ShieldSkill.cpp`

| Aspek | Value |
|-------|-------|
| Durasi | Instant (1 hit) |
| Cooldown | 30 detik |
| Efek | Menahan 1 tabrakan asteroid |

Saat shield aktif dan asteroid menabrak player: shield hancur (consumed), asteroid hancur, player selamat.

### 5.2 Aura Field

| Aspek | Value |
|-------|-------|
| Durasi | 10 detik |
| Cooldown | 30 detik |
| Radius | Lebar dari hitbox normal |
| Efek | Hancurkan semua asteroid yang menyentuh aura |

### 5.3 Bomb / Shockwave

**File:** `src/BombSkill.cpp`

| Aspek | Value |
|-------|-------|
| Cooldown | 30 detik |
| Durasi ledakan | 0.5 detik |
| Radius max | 1100 px (seluruh layar) |

**Cara kerja:**
1. Player aktivasi → state EXPLODING
2. Selama 0.5 detik, radius shockwave membesar dari 0 → 1100 px
3. Semua asteroid dalam radius hancur
4. Setiap asteroid yang hancur → **AddScore(panjang kata)** (fitur baru)
5. Cooldown 30 detik

**Visual:** 2 lingkaran konsentris (putih + cyan) dengan alpha memudar.

### 5.4 Instant Crit

| Aspek | Value |
|-------|-------|
| Durasi | 10 detik |
| Cooldown | 30 detik |
| Efek | Huruf pertama langsung hancurkan asteroid |

Saat aktif: player cukup ketik 1 huruf pertama yang cocok, asteroid langsung hancur tanpa harus mengetik sisa kata.

### 5.5 Score Booster

| Aspek | Value |
|-------|-------|
| Durasi | 5 detik |
| Cooldown | 30 detik |
| Multiplier | 16× |

Saat aktif: semua score dikali 16. Cocok dipakai pas combo tinggi untuk skor maksimal.

---

## 6. Difficulty Manager

**File:** `src/DifficultyManager.cpp`, `include/DifficultyManager.h`

Kesulitan naik seiring waktu berjalan:

| Waktu | Tier Kata | Jumlah Shower |
|-------|-----------|---------------|
| 0-20 detik | Tier 1-2 (Easy) | 5 asteroid |
| 20-40 detik | Tier 1-3 | 5 asteroid |
| 40-60 detik | Tier 2-4 | 5 asteroid |
| 60+ detik | Tier 1-5 | 6+ asteroid |
| Makin lama | Maks Tier 1-6 | Maks 15 asteroid |

---

## 7. ExplosionManager (Efek Ledakan)

**File:** `src/ExplosionManager.cpp`, `include/ExplosionManager.h`

Sistem partikel sederhana dengan **pool 64 ledakan**, masing-masing maks 35 partikel.

```cpp
struct Explosion {
    Vector2 pos;
    Color color;
    float timer, duration = 0.5f;
    int particleCount;
    struct Particle { Vector2 pos, vel; float radius; } particles[35];
    bool active;
};
```

Tiap partikel:
- Bergerak dengan arah random (0-360°)
- Kecepatan 80-300 px/s
- Radius 3-8 px
- Melambat 2% per frame (damping 0.98)
- Alpha memudar seiring waktu

---

## 8. AsteroidManager

**File:** `src/AsteroidManager.cpp`, `include/AsteroidManager.h`

### Komponen Utama

| Komponen | Struktur Data | Kapasitas |
|----------|---------------|-----------|
| Pool asteroid normal | Array fixed-size | 50 asteroid |
| Shower asteroid | SinglyLinkedList | Dinamis (bertambah) |
| Event queue | Priority Queue | Events (SPAWN_NORMAL, SPAWN_SHOWER) |

### Priority Queue Event

Ada 2 jenis event:
- **NORMAL** — spawn asteroid biasa, interval ~2 detik
- **SHOWER** — hujan asteroid bertubi-tubi, interval ~30 detik

**Prioritas:** SHOWER > NORMAL (event shower selalu dieksekusi lebih dulu).

### SinglyLinkedList untuk Shower

**File:** `include/SinglyLinkedList.h`

Template class `SinglyLinkedList<T>` dengan operasi:
- `push_front()`, `push_back()` — tambah node
- `pop_front()`, `pop_back()` — hapus node
- `getCurrent()`, `next()` — navigasi cursor (untuk shower wave)
- `remove(value)` — cari & hapus

Setiap 60 detik, node baru ditambahkan ke list shower. Saat shower dipicu, asteroid dari linked list diaktifkan satu per satu dengan interval 0.2 detik.

---

## 9. Scanning Asteroid dengan Lambda

**File:** `AsteroidManager.cpp:32-55`

Fungsi `scanAllAsteroids` dan `scanAsteroid` menggunakan **Lambda function** (C++11):

```cpp
std::vector<Asteroid*> AsteroidManager::scanAllAsteroids(
    std::function<bool(const Asteroid&)> predicate) {
    
    std::vector<Asteroid*> result;
    for (auto &ast : poolAsteroid) {
        if (predicate(ast)) result.push_back(&ast);
    }
    for (auto* node = asteroidShower.getHead(); node; node = node->next) {
        if (predicate(node->data)) result.push_back(&node->data);
    }
    return result;
}
```

Contoh penggunaan lambda:
```cpp
// Cari asteroid dalam radius shockwave
auto hit = asteroidManager.scanAllAsteroids([r](const Asteroid& a) {
    return a.active && jarak(a.position, playerPos) <= r;
});

// Cari asteroid dengan huruf pertama 'A'
auto targets = asteroidManager.scanAllAsteroids([c](const Asteroid& ast) {
    return ast.active && !ast.word.empty() && ast.word[0] == c;
});
```

---

## 10. Game Over Handling

Saat asteroid menabrak player:
1. `isHit()` — scan collision dengan hitbox player
2. Jika Aura aktif → asteroid hancur, player aman
3. Jika Shield aktif → shield consumed, asteroid hancur
4. Jika tidak ada skill → `triggerGameOver()`:
   - Play sound gameover
   - Ledakan besar di posisi player
   - Return true → game over

Setelah game over:
- Score disimpan
- Score → RP (score / 100)
- Update highest score
- Profile disimpan ke JSON

---

## 11. File yang Relevan

| File | Baris | Kegunaan |
|------|-------|----------|
| `src/GameplayManager.cpp` | 390 | Inti gameplay: typing state machine, skill, scoring |
| `include/GameplayManager.h` | 102 | Header gameplay manager |
| `src/Asteroid.cpp` | 153 | Logika asteroid (typing, movement, render) |
| `include/Asteroid.h` | 37 | Struct Asteroid |
| `src/AsteroidManager.cpp` | 271 | Pool, shower, event queue |
| `include/AsteroidManager.h` | 78 | Header asteroid manager |
| `src/SpaceShip.cpp` | 84 | Render spaceship + laser |
| `include/SpaceShip.h` | 40 | Header spaceship |
| `src/ComboStack.cpp` | 27 | Stack combo (fixed array) |
| `include/ComboStack.h` | 16 | Header combo |
| `src/ExplosionManager.cpp` | 76 | Sistem partikel ledakan |
| `include/ExplosionManager.h` | 32 | Header explosion |
| `src/BombSkill.cpp` | 75 | Skill shockwave |
| `include/BombSkill.h` | 23 | Header bomb |
| `src/ShieldSkill.cpp` | 77 | Skill shield |
| `src/AuraFieldSkill.cpp` | 73 | Skill aura |
| `src/InstantCritSkill.cpp` | 73 | Skill instant crit |
| `src/ScoreBoosterSkill.cpp` | 73 | Skill score booster |
| `include/Skill.h` | 14 | Abstract base class skill |
| `include/SinglyLinkedList.h` | 81 | Template SinglyLinkedList |
| `src/DifficultyManager.cpp` | 42 | Manajemen kesulitan waktu |
| `include/DifficultyManager.h` | 11 | Header difficulty |
| `src/WordGenerator.cpp` | 34 | Random word picker |
| `include/WordGenerator.h` | 15 | Header word generator |
| `include/word_module.h` | 253 | 150 words (Easy/Medium/Hard) |
| `include/GameConfig.h` | 44 | Semua konstanta game |

---

## 12. Jawaban untuk Tanya Jawab

| Pertanyaan | Jawaban |
|------------|---------|
| Gimana cara sistem typing bekerja? | State machine 2 state: SEARCH (cari target) → TARGET_LOCKED (ketik kata) |
| Berapa skor per karakter? | 1 poin × combo multiplier × (16 jika score booster aktif) |
| Struktur data apa yang dipakai untuk combo? | Stack dengan fixed array max 6 level |
| Struktur data apa untuk pool asteroid? | Array fixed-size 50 |
| Struktur data apa untuk shower asteroid? | SinglyLinkedList (template) |
| Kenapa perlu Lambda function? | Untuk scanning asteroid dengan kondisi fleksibel (filter radius, huruf, dll) |
| Skill apa yang bisa dipakai saat gameplay? | 5 skill: Shield, Aura, Shockwave, Instant Crit, Score Booster |
| Tombol apa untuk aktivasi skill? | KEY_1 sampai KEY_N (otomatis berdasarkan urutan unlock) |
| Bagaimana kesulitan meningkat? | Semakin lama bermain, tier asteroid naik (1→6) dan jumlah shower bertambah |
| Berapa cooldown shockwave? | 30 detik, radius mencakup seluruh layar |
