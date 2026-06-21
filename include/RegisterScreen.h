#pragma once
#include "raylib.h"
#include "DataManager.h"
#include <string>

// Layar registrasi akun baru.
// Validasi username unik, simpan ke database lokal (PlayerData.json).
class RegisterScreen {
public:
    RegisterScreen();
    void Update();          // Tangani input + submit registrasi
    void Draw();            // Render form registrasi
    void Reset();           // Reset input dan status

    bool IsRegisterSuccess() const;     // Return true jika registrasi berhasil
    bool ShouldGoToLogin() const;       // Return true saat ESC (kembali ke login)
    const std::string& GetRegisteredUsername() const; // Username yang baru terdaftar

private:
    char inputBuffer[32];   // Buffer input username
    int inputLen;           // Panjang input saat ini
    std::string statusMessage;  // Pesan error/sukses
    Color statusColor;      // Warna pesan status (MERAH = error, HIJAU = sukses)

    bool isRegisterSuccess; // Flag: registrasi berhasil
    bool goToLogin;         // Flag: kembali ke login
    std::string registeredUsername; // Username yang berhasil didaftarkan
};
