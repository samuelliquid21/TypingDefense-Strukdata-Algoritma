#include "RegisterScreen.h"

// ===============================
// 📝 LAYAR REGISTER
// ===============================

RegisterScreen::RegisterScreen() {
    Reset();
}

// Reset semua input dan status ke nilai awal
void RegisterScreen::Reset() {
    inputBuffer[0] = '\0';
    inputLen = 0;
    statusMessage = "";
    statusColor = RED;
    isRegisterSuccess = false;
    goToLogin = false;
    registeredUsername = "";
}

// Tangani input keyboard: ketik username, validasi, submit registrasi
void RegisterScreen::Update() {
    // Baca karakter yang diketik
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 125 && inputLen < 31) {
            inputBuffer[inputLen] = (char)key;
            inputLen++;
            inputBuffer[inputLen] = '\0';
        }
        key = GetCharPressed();
    }

    // Hapus karakter terakhir dengan BACKSPACE
    if (IsKeyPressed(KEY_BACKSPACE) && inputLen > 0) {
        inputLen--;
        inputBuffer[inputLen] = '\0';
        statusMessage = "";
    }

    // Submit registrasi dengan ENTER
    if (IsKeyPressed(KEY_ENTER) && inputLen > 0) {
        std::string username(inputBuffer);
        PlayerProfile dummyProfile;

        // Cek apakah username sudah terdaftar
        if (DataManager::getInstance().FindPlayer(username, dummyProfile)) {
            statusMessage = "Username already taken!";
            statusColor = RED;
        } else {
            // Buat akun baru di database
            DataManager::getInstance().CreatePlayer(username);

            registeredUsername = username;
            isRegisterSuccess = true;
            statusMessage = "Registration Successful!";
            statusColor = GREEN;
        }
    }

    // ESC → kembali ke login
    if (IsKeyPressed(KEY_ESCAPE)) {
        goToLogin = true;
    }
}

// Render form registrasi: judul, label, kotak input, hint
void RegisterScreen::Draw() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Judul
    int titleFontSize = 50;
    const char* titleText = "REGISTER";
    int titleWidth = MeasureText(titleText, titleFontSize);
    DrawText(titleText, (screenWidth - titleWidth) / 2, screenHeight / 2 - 150, titleFontSize, WHITE);

    // Label
    int labelFontSize = 20;
    const char* labelText = "Create new username:";
    int labelWidth = MeasureText(labelText, labelFontSize);
    DrawText(labelText, (screenWidth - labelWidth) / 2, screenHeight / 2 - 60, labelFontSize, LIGHTGRAY);

    // Kotak input
    int boxWidth = 400;
    int boxHeight = 50;
    int boxX = (screenWidth - boxWidth) / 2;
    int boxY = screenHeight / 2 - 30;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(DARKGRAY, 0.8f));
    DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, WHITE);

    // Teks input user
    int inputFontSize = 24;
    int inputWidth = MeasureText(inputBuffer, inputFontSize);
    DrawText(inputBuffer, boxX + (boxWidth - inputWidth) / 2, boxY + (boxHeight - inputFontSize) / 2, inputFontSize, WHITE);

    // Pesan status (error/sukses)
    if (!statusMessage.empty()) {
        int statusWidth = MeasureText(statusMessage.c_str(), 16);
        DrawText(statusMessage.c_str(), (screenWidth - statusWidth) / 2, boxY + boxHeight + 15, 16, statusColor);
    }

    // Hint navigasi
    const char* hint1 = "Press ENTER to register";
    const char* hint2 = "[ESC] Back to Login";

    DrawText(hint1, (screenWidth - MeasureText(hint1, 16)) / 2, boxY + boxHeight + 70, 16, GRAY);
    DrawText(hint2, (screenWidth - MeasureText(hint2, 16)) / 2, boxY + boxHeight + 100, 16, LIGHTGRAY);
}

bool RegisterScreen::IsRegisterSuccess() const { return isRegisterSuccess; }
bool RegisterScreen::ShouldGoToLogin() const { return goToLogin; }
const std::string& RegisterScreen::GetRegisteredUsername() const { return registeredUsername; }
