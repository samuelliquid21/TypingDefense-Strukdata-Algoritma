#pragma once
#include "raylib.h"

// Layar logout dengan penundaan singkat.
// Setelah 1 detik, status logout selesai dan game kembali ke login.
class LogoutScreen {
public:
    LogoutScreen();
    void Update();          // Hitung mundur timer logout
    void Draw();            // Tampilkan pesan logout berhasil
    void Reset();           // Reset timer

    bool IsFinished() const; // Apakah proses logout sudah selesai

private:
    float m_logoutTimer;    // Timer hitung mundur (detik)
    bool m_isFinished;      // Flag: logout selesai
};
