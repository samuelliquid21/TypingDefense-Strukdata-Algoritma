#pragma once
#include "raylib.h"
#include "DataManager.h"
#include <string>

// Layar login untuk memasukkan username pemain.
// Validasi dilakukan terhadap database lokal (PlayerData.json).
class LoginScreen {
public:
    LoginScreen();
    void Update();          // Tangani input keyboard + validasi
    void Draw();            // Render form login
    void Reset();           // Reset input dan status

    bool ShouldLogin() const;           // Return true saat ENTER + username valid
    bool ShouldGoToRegister() const;    // Return true saat TAB (pindah ke register)
    bool ShouldGoBack() const;          // Return true saat ESC (keluar game)
    const PlayerProfile& GetProfile() const; // Profile yang berhasil login

private:
    char inputBuffer[32];   // Buffer input username
    int inputLen;           // Panjang input saat ini
    std::string statusMessage;  // Pesan error/sukses
    PlayerProfile loggedInProfile; // Profile hasil login
    bool shouldLogin;       // Flag: login berhasil
    bool goToRegister;      // Flag: pindah ke register
    bool goBack;            // Flag: kembali/keluar

    void handleTextInput();     // Baca karakter dari keyboard
    void handleBackspace();     // Hapus karakter terakhir
    void handleSubmit();        // Submit login dengan ENTER
    void handleNavigationKeys(); // TAB ke register, ESC untuk quit
    void drawLoginForm();       // Render title, label, kotak input
    void drawLoginTitle();      // Render judul "LOGIN"
    void drawLoginLabel();      // Render label "Enter username:"
    void drawLoginInputBox();   // Render kotak input dan teks user
    void drawStatusMessage();   // Render pesan error
    void drawNavigationHints(); // Render hint navigasi
};
