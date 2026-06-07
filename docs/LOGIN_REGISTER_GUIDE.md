# Panduan Implementasi Login, Register & Logout

## Overview

Sistem player profile sudah tersedia di `DataManager` (singleton). Yang perlu diimplementasikan hanya **UI** dan **logika transisi state** di `Game`.

## Arsitektur yang Sudah Ada

| Komponen | Lokasi | Fungsi |
|---|---|---|
| `PlayerProfile` struct | `include/DataManager.h` | Menyimpan data player (username, score, RP, unlocked words/skills) |
| `DataManager::FindPlayer()` | `include/DataManager.h` | Mencari player di JSON, return `true` jika ketemu |
| `DataManager::CreatePlayer()` | `include/DataManager.h` | Membuat entry baru di JSON |
| `DataManager::SavePlayer()` | `include/DataManager.h` | Menyimpan perubahan data player ke JSON |
| `m_currentPlayer` | `include/Game.h` | Menyimpan data player yang sedang aktif |
| `m_isLoggedIn` | `include/Game.h` | Flag apakah ada player yang login |

## Game State yang Tersedia

State untuk login/register dan logout sudah didefinisikan di `include/State.h`:
- `GameState::LOGIN_AND_REGISTER` → fungsi kosong: `UpdateLoginRegister()` dan `DrawLoginRegister()`
- `GameState::LOGOUT` → fungsi kosong: `UpdateLogout()` dan `DrawLogout()`

Semua fungsi ini ada di `src/Game.cpp` dan saat ini **kosong**.

---

## 1. Implementasi Login

### Langkah

1. Di `UpdateLoginRegister()`, buat input untuk username (gunakan raylib text input)
2. Saat user submit (tekan Enter), panggil:

```cpp
PlayerProfile profile;
bool found = DataManager::getInstance().FindPlayer(username, profile);

if (found) {
    m_currentPlayer = profile;
    m_isLoggedIn = true;
    state = GameState::MENU;
} else {
    // Username tidak ditemukan, arahkan ke register atau buat baru
    // Bisa juga langsung CreatePlayer lalu login
}
```

### Contoh Implementasi Sederhana

```cpp
// Di Game.h, tambahkan:
char loginInput[32] = {0};
int loginInputLen = 0;
bool isLoginInputActive = false;

// Di UpdateLoginRegister():
if (isLoginInputActive) {
    // Gunakan raylib text input
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 125 && loginInputLen < 31) {
            loginInput[loginInputLen] = (char)key;
            loginInputLen++;
            loginInput[loginInputLen] = '\0';
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_ENTER)) {
        std::string username(loginInput);
        if (!username.empty()) {
            PlayerProfile profile;
            if (DataManager::getInstance().FindPlayer(username, profile)) {
                m_currentPlayer = profile;
                m_isLoggedIn = true;
                // Reset input
                loginInput[0] = '\0';
                loginInputLen = 0;
                state = GameState::MENU;
            } else {
                // Tampilkan pesan "Username tidak ditemukan"
            }
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (loginInputLen > 0) {
            loginInput[loginInputLen - 1] = '\0';
            loginInputLen--;
        }
    }
}

// Di DrawLoginRegister():
DrawText("LOGIN", 400, 200, 30, WHITE);
DrawText("Masukkan username:", 350, 280, 20, LIGHTGRAY);
DrawRectangle(350, 320, 300, 40, DARKGRAY);
DrawText(loginInput, 360, 330, 20, WHITE);
DrawText("Tekan Enter untuk login", 350, 380, 16, GRAY);
```

---

## 2. Implementasi Register

### Langkah

1. Di `UpdateLoginRegister()`, buat tab/menu untuk register
2. Saat user submit username baru, panggil:

```cpp
// Cek apakah username sudah ada
PlayerProfile dummy;
bool exists = DataManager::getInstance().FindPlayer(username, dummy);

if (!exists) {
    DataManager::getInstance().CreatePlayer(username);
    // Auto login setelah register
    DataManager::getInstance().FindPlayer(username, m_currentPlayer);
    m_isLoggedIn = true;
    state = GameState::MENU;
} else {
    // Username sudah dipakai
}
```

### Contoh Implementasi Sederhana

```cpp
// Di Game.h, tambahkan:
char registerInput[32] = {0};
int registerInputLen = 0;
bool isRegisterMode = false;

// Di UpdateLoginRegister():
if (isRegisterMode) {
    // Sama seperti login, tapi panggil CreatePlayer
    if (IsKeyPressed(KEY_ENTER)) {
        std::string username(registerInput);
        if (!username.empty()) {
            PlayerProfile dummy;
            if (!DataManager::getInstance().FindPlayer(username, dummy)) {
                DataManager::getInstance().CreatePlayer(username);
                DataManager::getInstance().FindPlayer(username, m_currentPlayer);
                m_isLoggedIn = true;
                state = GameState::MENU;
            } else {
                // Username sudah ada
            }
        }
    }
}

// Di DrawLoginRegister():
if (!isRegisterMode) {
    DrawText("Login", 400, 200, 30, WHITE);
    // ... draw login UI
    // Tombol switch ke register
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, Rectangle{350, 420, 300, 40})) {
            isRegisterMode = true;
        }
    }
    DrawText("Belum punya akun? Daftar", 350, 420, 18, LIGHTGRAY);
} else {
    DrawText("Register", 400, 200, 30, WHITE);
    // ... draw register UI
    // Tombol kembali ke login
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, Rectangle{350, 420, 300, 40})) {
            isRegisterMode = false;
        }
    }
    DrawText("Sudah punya akun? Login", 350, 420, 18, LIGHTGRAY);
}
```

---

## 3. Implementasi Logout

### Langkah

1. Di `UpdateLogout()`, reset state player:

```cpp
m_currentPlayer = PlayerProfile{};
m_isLoggedIn = false;
state = GameState::MENU;
```

2. Di `DrawLogout()`, tampilkan konfirmasi:

```cpp
DrawText("Logout berhasil!", 400, 300, 30, WHITE);
DrawText("Kembali ke menu...", 380, 350, 20, LIGHTGRAY);
```

### Contoh Implementasi

```cpp
void Game::UpdateLogout() {
    // Tampilkan konfirmasi sebentar, lalu logout
    logoutTimer += GetFrameTime();
    if (logoutTimer >= 1.0f) {
        m_currentPlayer = PlayerProfile{};
        m_isLoggedIn = false;
        logoutTimer = 0.0f;
        state = GameState::MENU;
    }
}

// Di Game.h, tambahkan:
float logoutTimer = 0.0f;
```

---

## 4. Menghubungkan ke Menu Utama

Saat ini di `UpdateMenu()`, tombol pertama langsung masuk gameplay. Setelah login/register ada, ubah logikanya:

```cpp
// Di UpdateMenu(), sebelum memulai gameplay:
if (!m_isLoggedIn) {
    // Arahkan ke halaman login/register
    state = GameState::LOGIN_AND_REGISTER;
    return;
}
```

Atau tambahkan tombol "Login/Logout" di `MainMenu`:

```cpp
// Jika ada tombol Login/Logout di menu:
if (choice == 4) { // Tombol login/logout
    if (m_isLoggedIn) {
        state = GameState::LOGOUT;
    } else {
        state = GameState::LOGIN_AND_REGISTER;
    }
}
```

---

## 5. Menyimpan Data Player saat Game Over

Saat game over, update `highest_score` dan simpan:

```cpp
// Di UpdateGameOver() atau saat transisi ke game over:
if (m_isLoggedIn) {
    if (score > m_currentPlayer.highest_score) {
        m_currentPlayer.highest_score = score;
    }
    // Tambah research point berdasarkan score
    m_currentPlayer.research_point += score / 100;
    DataManager::getInstance().SavePlayer(m_currentPlayer);
}
```

---

## 6. Menghapus Kode Debug

Setelah login/register selesai, hapus baris ini di `src/Game.cpp` constructor:

```cpp
// HAPUS INI:
// TODO: Hapus saat login/register sudah ada
DataManager::getInstance().FindPlayer("hyperion", m_currentPlayer);
m_isLoggedIn = true;
```

---

## Checklist Implementasi

- [ ] UI input username untuk login (`DrawLoginRegister`)
- [ ] Logika validasi login (`UpdateLoginRegister`)
- [ ] UI input username untuk register (`DrawLoginRegister`)
- [ ] Logika pembuatan player baru (`UpdateLoginRegister`)
- [ ] Konfirmasi logout (`UpdateLogout`, `DrawLogout`)
- [ ] Redirect ke login jika belum login di menu
- [ ] Simpan data player saat game over
- [ ] Hapus kode debug di constructor `Game`
