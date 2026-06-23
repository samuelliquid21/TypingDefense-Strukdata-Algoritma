# Materi Presentasi 1 — Login, Main Menu, dan Flow Aplikasi

## 📋 Ringkasan

Bagian ini membahas alur program dari awal dijalankan hingga masuk ke gameplay dan fitur lainnya. Mulai dari login/register, menu utama dengan navigasi, transisi antar layar, sampai mekanisme pause dan game over.

---

## 1. Entry Point (main.cpp)

**File:** `src/main.cpp`

Program dimulai dari `main()`:

```cpp
int main() {
    Game game;
    game.Run();
    return 0;
}
```

Cuma 3 baris: bikin objek `Game`, lalu jalanin loop utama `Run()`. Semua inisialisasi terjadi di constructor `Game`.

---

## 2. Inisialisasi Game (Game.cpp)

**File:** `src/Game.cpp`

Saat `Game` dibuat, terjadi:
1. **Init window + audio** — `InitWindow(1080, 720, "Cosmic Keypad - Kelompok 4")`, `InitAudioDevice()`
2. **Load data player** — `DataManager::getInstance().load()` baca file `data/PlayerData.json`
3. **Load asset** — register spritesheet spaceship, load background
4. **Load tech tree** — skill apa saja yang sudah di-unlock
5. **Setup callback** — fungsi yang dipanggil saat asteroid hancur (untuk unlock kata + RP)
6. **Init leaderboard** — siapkan AVL Tree

State awal: `MENU`.

### Game State Machine

Ada **19 state** yang diatur di `include/State.h`:

| State | Kegunaan |
|-------|----------|
| `MENU` | Menu utama |
| `GAMEPLAY` | Permainan berjalan |
| `PAUSE` | Game dijeda |
| `GAME_OVER` | Game over screen |
| `LEADERBOARD` | Papan peringkat |
| `CREDIT` | Layar kredit |
| `LOGIN_AND_REGISTER` | Login |
| `REGISTER` | Registrasi akun baru |
| `LOGOUT` | Keluar dari sesi |
| `UNLOCK_SKILL` | Tech tree / skill lab |
| `WORD_DICTIONARY` | Kamus kata |
| `UNLOCKED_WORDS` | Kata yang sudah di-unlock |
| `SKIN_SELECT` | Pilih skin / beli skin |
| `GACHA` | Gacha crate |

### Game Loop

```cpp
void Game::Run() {
    while (!WindowShouldClose() && !statusMenuQuit) {
        Update();   // Update logika sesuai state
        Draw();     // Render tampilan sesuai state
    }
    // Cleanup: simpan data, unload resource, tutup window
}
```

Tiap frame, `Update()` dan `Draw()` di-delegate ke method yang sesuai berdasarkan state aktif (switch-case di `dispatchUpdateState()` dan `dispatchDrawState()`).

### Transisi Glitch

**File:** `src/TransitionEffect.cpp`, `include/TransitionEffect.h`

Setiap pindah state (misal dari menu ke gameplay), ada efek **glitch**:
- Garis-garis horizontal cyan/magenta acak
- Suara glitch effect
- Berlangsung ~0.3 detik

Ini membuat transisi terasa lebih halus dan futuristik.

---

## 3. Login & Register

### Login Screen

**File:** `src/LoginScreen.cpp`, `include/LoginScreen.h`

Fungsi:
- User ketik username di kotak input
- Tekan ENTER → cek ke database (`DataManager::FindPlayer()`)
- Kalau ketemu → login sukses, simpan profile, pindah ke MENU
- Kalau tidak → tampil error "Username not found!"
- TAB → pindah ke Register
- ESC → keluar game

Navigasi keyboard:
| Tombol | Aksi |
|--------|------|
| Huruf/angka | Ketik username |
| BACKSPACE | Hapus karakter |
| ENTER | Submit login |
| TAB | Buka Register |
| ESC | Keluar |

### Register Screen

**File:** `src/RegisterScreen.cpp`, `include/RegisterScreen.h`

Fungsi:
- User ketik username baru
- Tekan ENTER
- Cek apakah username sudah dipakai (`FindPlayer()`)
- Kalau sudah → error "Username already taken!"
- Kalau belum → buat akun baru (`CreatePlayer()`), simpan, langsung login
- ESC → kembali ke Login

### Data Player

**File:** `include/DataManager.h`, `src/DataManager.cpp`

Data disimpan di file `data/PlayerData.json`. Struktur per player:

```json
{
    "username": "hyperion",
    "highest_score": 2500,
    "research_point": 150,
    "survival_time": 120.5,
    "unlocked_words": ["ASTEROID", "SPACE"],
    "unlocked_skills": ["barrier", "shockwave"]
}
```

Operasi utama:
- `FindPlayer(username, profile)` — cari player, return profile-nya
- `SavePlayer(profile)` — update data player di JSON
- `CreatePlayer(username)` — buat entri baru dengan default values

Ada **exception handling** (try-catch) untuk:
- Parse error saat baca JSON (file corrupt)
- Type error saat akses field (struktur tidak sesuai)
- Error saat menyimpan file (disk penuh, permission)

---

## 4. Main Menu

**File:** `src/MainMenu.cpp`, `include/MainMenu.h`

Menu utama menggunakan **Circular Doubly Linked List** sebagai struktur data navigasi.

### Circular Doubly Linked List

```
          ┌──────────────────────────────────────────┐
          │                                          ↓
    [Play] ⇄ [Leaderboard] ⇄ [Skins] ⇄ [Skills] ⇄ [Word Bank] ⇄ [Credit] ⇄ [Logout] ⇄ [Quit]
      ↑                                                                                   │
      └───────────────────────────────────────────────────────────────────────────────────┘
```

Setiap node (`MenuNode`) punya:
- `data` — teks menu (string)
- `prev` — pointer ke node sebelumnya (navigasi UP)
- `next` — pointer ke node berikutnya (navigasi DOWN)

**Kenapa Circular DLL?**
- Navigasi melingkar (dari Play ke Quit dan sebaliknya) berjalan mulus
- Tidak perlu boundary check untuk wrap-around
- Struktur data ini adalah implementasi dari materi Circular Doubly Linked List

Operasi pada list:
| Operasi | File | Baris | Kompleksitas |
|---------|------|-------|-------------|
| `AddOption(teks)` | MainMenu.cpp:41 | Membuat node baru, link di akhir list | O(1) |
| `ClearOptions()` | MainMenu.cpp:64 | Hapus semua node | O(n) |
| Navigasi UP/DOWN | MainMenu.cpp:78-94 | Geser current ke prev/next | O(1) |

### Flow Menu

```
Game Start → Login Screen → [Login / Register] → MENU
                                                     │
                    ┌────────────────────────────────┼────────────────────────────┐
                    ↓                                ↓                            ↓
              [0] Play → Gameplay              [1] Leaderboard              [2] Skins
                    ↓                                ↓                            ↓
              Game Over ← (collision)           Tampilkan AVL Tree         Skin Select / Gacha
                    ↓
              Pilihan: [Resume / Back to Menu] ← PAUSE (ESC)

              [3] Skills (Tech Tree)    [4] Word Bank    [5] Credit    [6] Logout    [7] Quit
```

### Flow Login/Logout

1. **Pertama masuk** → state langsung `LOGIN_AND_REGISTER`
2. **Login sukses** → simpan `m_currentPlayer`, `m_isLoggedIn = true`, pindah ke MENU
3. **Logout** (dari menu) → proses di `UpdateLogout()` selama 1 detik, reset semua data, kembali ke Login
4. **Session backup** — saat mulai gameplay, data player di-backup (`m_sessionBackup`). Kalau player memilih "Back to Menu" dari pause, session di-restore (undo semua perubahan).

---

## 5. Pause

**File:** `src/Pause.cpp`, `include/Pause.h`

Saat player tekan ESC di gameplay:
- Tampilkan menu **PAUSED** dengan 2 opsi:
  1. **Resume Game** — countdown 3-2-1-GO lalu lanjut
  2. **Back to Menu** — undo session, balik ke menu utama
- Overlay transparan di belakang menu pause
- Countdown dengan angka besar di tengah layar

---

## 6. Game Over

**File:** `src/GameOver.cpp`, `include/GameOver.h`

Saat player terkena asteroid:
1. Simpan score akhir
2. Konversi score → Research Points (score / 100)
3. Update highest score jika rekor
4. Simpan profile ke JSON
5. Tampilkan score di layar
6. ENTER untuk kembali ke menu

---

## 7. Flow Chart Aplikasi

```
                    ┌──────────────────────────────────────────────────────┐
                    │                   main.cpp                          │
                    │                   Game game;                        │
                    │                   game.Run();                       │
                    └────────────────────┬─────────────────────────────────┘
                                         ↓
                    ┌──────────────────────────────────────────────────────┐
                    │              Game constructor                        │
                    │  InitWindow() | InitAudio() | LoadAssets()          │
                    │  LoadPlayerData() | InitTechTree() | InitLeaderboard │
                    └────────────────────┬─────────────────────────────────┘
                                         ↓
                    ┌──────────────────────────────────────────────────────┐
                    │              LOGIN_AND_REGISTER state                │
                    │  LoginScreen / RegisterScreen                        │
                    └────────────────────┬─────────────────────────────────┘
                                         ↓ (login sukses)
                    ┌──────────────────────────────────────────────────────┐
                    │                    MENU state                        │
                    │  MainMenu (Circular DLL)                             │
                    │  8 opsi: Play | LB | Skins | Skills                 │
                    │  | Word Bank | Credit | Logout | Quit                │
                    └──┬────┬────┬────┬────┬────┬────┬────┬────────────────┘
                       ↓    ↓    ↓    ↓    ↓    ↓    ↓    ↓
                      [0]  [1]  [2]  [3]  [4]  [5]  [6]  [7]
                       ↓    ↓    ↓    ↓    ↓    ↓    ↓    ↓
                   Game- Leader Skin  Tech  Word  Credit Logout Quit
                   play  board Select Tree  Bank
```

## 8. File yang Relevan

| File | Kegunaan |
|------|----------|
| `src/main.cpp` | Entry point program |
| `src/Game.cpp` | State machine utama (614 baris) |
| `include/Game.h` | Header Game class |
| `src/LoginScreen.cpp` | Form login (155 baris) |
| `include/LoginScreen.h` | Header login |
| `src/RegisterScreen.cpp` | Form registrasi (157 baris) |
| `include/RegisterScreen.h` | Header register |
| `src/MainMenu.cpp` | Menu utama Circular DLL (140 baris) |
| `include/MainMenu.h` | Header main menu |
| `src/LogoutScreen.cpp` | Logout screen (33 baris) |
| `include/LogoutScreen.h` | Header logout |
| `src/Pause.cpp` | Menu pause + countdown (87 baris) |
| `include/Pause.h` | Header pause |
| `src/GameOver.cpp` | Game over screen (53 baris) |
| `include/GameOver.h` | Header game over |
| `src/DataManager.cpp` | Manajemen data JSON (127 baris) |
| `include/DataManager.h` | Header data manager |
| `include/State.h` | Enum GameState (19 state) |
| `src/TransitionEffect.cpp` | Efek glitch transisi (65 baris) |
| `include/TransitionEffect.h` | Header transisi |
| `data/PlayerData.json` | Database player |

## 9. Jawaban untuk Tanya Jawab

| Pertanyaan | Jawaban |
|------------|---------|
| Kenapa pake Circular DLL untuk menu? | Biar navigasi UP/DOWN bisa melingkar tanpa batasan, cocok untuk UI navigasi |
| Gimana cara simpan data player? | Pake JSON via library `nlohmann/json`, disimpan di `data/PlayerData.json` |
| Apa bedanya Score sama RP? | Score didapat saat gameplay (dari ngetik), RP adalah currency untuk unlock skill/skin (1 RP = 100 score) |
| Berapa banyak state di game? | 19 state | 
| Gimana cara atasi file JSON corrupt? | Pake try-catch, kalau error parsing data di-reset ke object kosong |
