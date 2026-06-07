# Panduan Implementasi Login, Register & Logout

## Overview

Sistem player profile sudah tersedia di `DataManager` (singleton). Yang perlu diimplementasikan hanya **UI** dan **logika transisi state**.

**Prinsip utama: gunakan pendekatan modular.** Jangan menumpuk semua kode di `Game.cpp`. Buat class terpisah untuk setiap layar (login, register, logout) agar kode tetap bersih dan mudah di-maintain.

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

## Pendekatan Modular (Direkomendasikan)

### Mengapa Modular?

Saat ini `Game.cpp` sudah berisi banyak state: menu, gameplay, pause, game over, leaderboard, credit, login, logout. Jika semua logika login/register ditulis langsung di `Game.cpp`, file ini akan menjadi sangat besar dan sulit di-maintain.

**Solusi:** Buat class terpisah untuk setiap layar, lalu `Game` hanya memanggil `Update()` dan `Draw()` dari class tersebut.

### Struktur File yang Disarankan

```
include/
  LoginScreen.h
  RegisterScreen.h
  LogoutScreen.h

src/
  LoginScreen.cpp
  RegisterScreen.cpp
  LogoutScreen.cpp
```

### Contoh Class LoginScreen

**`include/LoginScreen.h`:**
```cpp
#pragma once
#include "raylib.h"
#include "DataManager.h"
#include <string>

class LoginScreen {
public:
    LoginScreen();
    void Update();
    void Draw();
    void Reset();
    bool ShouldLogin() const;       // return true saat Enter ditekan + valid
    bool ShouldGoToRegister() const; // return true saat klik "Daftar"
    bool ShouldGoBack() const;       // return true saat klik "Kembali"
    PlayerProfile GetProfile() const; // return profile yang berhasil login

private:
    char inputBuffer[32];
    int inputLen;
    std::string statusMessage;
    PlayerProfile loggedInProfile;
    bool shouldLogin;
    bool goToRegister;
    bool goBack;
};
```

**`src/LoginScreen.cpp`:**
```cpp
#include "LoginScreen.h"

LoginScreen::LoginScreen() {
    Reset();
}

void LoginScreen::Reset() {
    inputBuffer[0] = '\0';
    inputLen = 0;
    statusMessage = "";
    shouldLogin = false;
    goToRegister = false;
    goBack = false;
}

void LoginScreen::Update() {
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 125 && inputLen < 31) {
            inputBuffer[inputLen] = (char)key;
            inputLen++;
            inputBuffer[inputLen] = '\0';
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && inputLen > 0) {
        inputLen--;
        inputBuffer[inputLen] = '\0';
        statusMessage = "";
    }

    if (IsKeyPressed(KEY_ENTER) && inputLen > 0) {
        std::string username(inputBuffer);
        PlayerProfile profile;
        if (DataManager::getInstance().FindPlayer(username, profile)) {
            loggedInProfile = profile;
            shouldLogin = true;
        } else {
            statusMessage = "Username tidak ditemukan!";
        }
    }

    // Cek klik tombol "Daftar" atau "Kembali" (implementasi dengan mouse)
}

void LoginScreen::Draw() {
    // Gambar UI login
    DrawText("LOGIN", 400, 200, 30, WHITE);
    DrawText("Masukkan username:", 350, 280, 20, LIGHTGRAY);
    DrawRectangle(350, 320, 300, 40, DARKGRAY);
    DrawText(inputBuffer, 360, 330, 20, WHITE);

    if (!statusMessage.empty()) {
        DrawText(statusMessage.c_str(), 350, 380, 16, RED);
    }

    DrawText("Tekan Enter untuk login", 350, 420, 16, GRAY);
    DrawText("[D] Daftar akun baru", 350, 450, 16, LIGHTGRAY);
    DrawText("[ESC] Kembali", 350, 480, 16, GRAY);

    if (IsKeyPressed(KEY_D)) {
        goToRegister = true;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        goBack = true;
    }
}

bool LoginScreen::ShouldLogin() const { return shouldLogin; }
bool LoginScreen::ShouldGoToRegister() const { return goToRegister; }
bool LoginScreen::ShouldGoBack() const { return goBack; }
PlayerProfile LoginScreen::GetProfile() const { return loggedInProfile; }
```

### Cara Menggunakan di Game

**`include/Game.h`:**
```cpp
#include "LoginScreen.h"
#include "RegisterScreen.h"
#include "LogoutScreen.h"

class Game {
private:
    // ... member lain ...
    LoginScreen loginScreen;
    RegisterScreen registerScreen;
    LogoutScreen logoutScreen;
};
```

**`src/Game.cpp`:**
```cpp
void Game::UpdateLoginRegister() {
    loginScreen.Update();
    if (loginScreen.ShouldLogin()) {
        m_currentPlayer = loginScreen.GetProfile();
        m_isLoggedIn = true;
        loginScreen.Reset();
        state = GameState::MENU;
    } else if (loginScreen.ShouldGoToRegister()) {
        registerScreen.Reset();
        state = GameState::REGISTER; // perlu tambah state ini
    } else if (loginScreen.ShouldGoBack()) {
        loginScreen.Reset();
        state = GameState::MENU;
    }
}

void Game::DrawLoginRegister() {
    loginScreen.Draw();
}
```

---

## 1. Implementasi Login (Tanpa Modular - Alternatif Sederhana)

Jika ingin tetap menulis langsung di `Game.cpp` (tidak modular), berikut caranya:

### Langkah

1. Di `Game.h`, tambahkan variabel input:
```cpp
char loginInput[32] = {0};
int loginInputLen = 0;
std::string loginStatusMessage;
bool isRegisterMode = false;
```

2. Di `UpdateLoginRegister()`, saat user submit (tekan Enter), panggil:

```cpp
PlayerProfile profile;
bool found = DataManager::getInstance().FindPlayer(username, profile);

if (found) {
    m_currentPlayer = profile;
    m_isLoggedIn = true;
    state = GameState::MENU;
} else {
    loginStatusMessage = "Username tidak ditemukan!";
}
```

### Contoh Implementasi Sederhana

```cpp
// Di UpdateLoginRegister():
if (!isRegisterMode) {
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
                loginInput[0] = '\0';
                loginInputLen = 0;
                loginStatusMessage = "";
                state = GameState::MENU;
            } else {
                loginStatusMessage = "Username tidak ditemukan!";
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
if (!isRegisterMode) {
    DrawText("LOGIN", 400, 200, 30, WHITE);
    DrawText("Masukkan username:", 350, 280, 20, LIGHTGRAY);
    DrawRectangle(350, 320, 300, 40, DARKGRAY);
    DrawText(loginInput, 360, 330, 20, WHITE);
    if (!loginStatusMessage.empty()) {
        DrawText(loginStatusMessage.c_str(), 350, 380, 16, RED);
    }
    DrawText("Tekan Enter untuk login", 350, 420, 16, GRAY);
}
```

---

## 2. Implementasi Register

### Langkah

1. Cek apakah username sudah ada
2. Jika belum, buat player baru via `CreatePlayer()`, lalu auto-login

```cpp
PlayerProfile dummy;
bool exists = DataManager::getInstance().FindPlayer(username, dummy);

if (!exists) {
    DataManager::getInstance().CreatePlayer(username);
    DataManager::getInstance().FindPlayer(username, m_currentPlayer);
    m_isLoggedIn = true;
    state = GameState::MENU;
} else {
    // Username sudah dipakai
}
```

### Contoh Implementasi (Non-Modular)

```cpp
// Variabel di Game.h:
char registerInput[32] = {0};
int registerInputLen = 0;
std::string registerStatusMessage;

// Di UpdateLoginRegister() (mode register):
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
            registerStatusMessage = "Username sudah dipakai!";
        }
    }
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

### Contoh Implementasi (Non-Modular)

```cpp
float logoutTimer = 0.0f;

void Game::UpdateLogout() {
    logoutTimer += GetFrameTime();
    if (logoutTimer >= 1.0f) {
        m_currentPlayer = PlayerProfile{};
        m_isLoggedIn = false;
        logoutTimer = 0.0f;
        state = GameState::MENU;
    }
}

void Game::DrawLogout() {
    DrawText("Logout berhasil!", 400, 300, 30, WHITE);
    DrawText("Kembali ke menu...", 380, 350, 20, LIGHTGRAY);
}
```

---

## 4. Menyimpan Data Player saat Game Over

**Sudah terimplementasi otomatis.** Saat game over, jika `m_isLoggedIn` aktif dan score lebih tinggi dari `highest_score` yang tersimpan, data akan otomatis di-update dan disimpan ke JSON.

Logika ini ada di `src/Game.cpp` → `UpdateGameOver()`:

```cpp
void Game::UpdateGameOver() {
    if (m_isLoggedIn && score > m_currentPlayer.highest_score) {
        m_currentPlayer.highest_score = score;
        DataManager::getInstance().SavePlayer(m_currentPlayer);
    }

    gameOver.Update();
    if (gameOver.ShouldReturnToMenu()) {
        state = GameState::MENU;
    }
}
```

### Opsional: Tambah Research Point

Jika ingin menambah research point setelah game over, tambahkan di `UpdateGameOver()`:

```cpp
if (m_isLoggedIn) {
    int earnedRP = score / 100; // 1 RP per 100 score
    if (earnedRP > 0) {
        m_currentPlayer.research_point += earnedRP;
    }
    if (score > m_currentPlayer.highest_score) {
        m_currentPlayer.highest_score = score;
    }
    DataManager::getInstance().SavePlayer(m_currentPlayer);
}
```

---

## 5. Menghubungkan ke Menu Utama

Saat ini di `UpdateMenu()`, tombol pertama langsung masuk gameplay. Setelah login/register ada, ubah logikanya:

```cpp
// Di UpdateMenu(), sebelum memulai gameplay:
if (!m_isLoggedIn) {
    // Arahkan ke halaman login/register
    state = GameState::LOGIN_AND_REGISTER;
    return;
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

## 7. State Tambahan yang Mungkin Diperlukan

Saat ini hanya ada `LOGIN_AND_REGISTER` dan `LOGOUT`. Jika ingin memisahkan login dan register menjadi state berbeda, tambahkan di `include/State.h`:

```cpp
enum class GameState {
    // ... state yang sudah ada ...
    LOGIN,
    REGISTER,
    LOGOUT
};
```

Lalu tambahkan handler di `Game.h` dan `Game.cpp`:
- `UpdateLogin()`, `DrawLogin()`
- `UpdateRegister()`, `DrawRegister()`

Dan jangan lupa update `switch` di `Game::Update()` dan `Game::Draw()`.

---

## Checklist Implementasi

- [ ] Tentukan pendekatan: modular (class terpisah) atau langsung di Game.cpp
- [ ] Jika modular: buat `LoginScreen.h/.cpp`, `RegisterScreen.h/.cpp`, `LogoutScreen.h/.cpp`
- [ ] UI input username untuk login
- [ ] Logika validasi login via `DataManager::FindPlayer()`
- [ ] UI input username untuk register
- [ ] Logika pembuatan player baru via `DataManager::CreatePlayer()`
- [ ] Konfirmasi logout
- [ ] Redirect ke login jika belum login di menu
- [ ] (Opsional) Tambah research point saat game over
- [ ] Hapus kode debug di constructor `Game`
