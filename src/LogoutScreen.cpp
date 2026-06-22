#include "LogoutScreen.h"

// ===============================
// 🚪 LAYAR LOGOUT
// ===============================

LogoutScreen::LogoutScreen() {
    Reset();
}

// Reset timer dan status logout
void LogoutScreen::Reset() {
    m_logoutTimer = 0.0f;
    m_isFinished = false;
}

// Hitung mundur 1 detik, lalu tandai logout selesai
void LogoutScreen::Update() {
    m_logoutTimer += GetFrameTime();

    if (m_logoutTimer >= 1.0f) {
        m_isFinished = true;
    }
}

// Tampilkan pesan logout berhasil
void LogoutScreen::Draw() {
    DrawText("Logout berhasil!", 400, 300, 30, WHITE);
}

bool LogoutScreen::IsFinished() const {
    return m_isFinished;
}
