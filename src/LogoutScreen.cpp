#include "LogoutScreen.h"

LogoutScreen::LogoutScreen() {
    Reset();
}

void LogoutScreen::Reset() {
    m_logoutTimer = 0.0f;
    m_isFinished = false;
}

void LogoutScreen::Update() {
    // Akumulasikan waktu berdasarkan frame rate game (delta time)
    m_logoutTimer += GetFrameTime();
    
    // Jika sudah mencapai 1 detik, tandai bahwa proses logout selesai
    if (m_logoutTimer >= 1.0f) {
        m_isFinished = true;
    }
}

void LogoutScreen::Draw() {
    DrawText("Logout berhasil!", 400, 300, 30, WHITE);
}

bool LogoutScreen::IsFinished() const {
    return m_isFinished;
}