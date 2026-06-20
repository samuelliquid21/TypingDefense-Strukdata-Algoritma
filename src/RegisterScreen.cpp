#include "RegisterScreen.h"

RegisterScreen::RegisterScreen() {
    Reset();
}

void RegisterScreen::Reset() {
    inputBuffer[0] = '\0';
    inputLen = 0;
    statusMessage = "";
    statusColor = RED;
    isRegisterSuccess = false;
    goToLogin = false;
    registeredUsername = "";
}

void RegisterScreen::Update() {
    // 1. Tangani input ketikan keyboard
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 125 && inputLen < 31) {
            inputBuffer[inputLen] = (char)key;
            inputLen++;
            inputBuffer[inputLen] = '\0';
        }
        key = GetCharPressed();
    }

    // 2. Tangani hapus karakter (Backspace)
    if (IsKeyPressed(KEY_BACKSPACE) && inputLen > 0) {
        inputLen--;
        inputBuffer[inputLen] = '\0';
        statusMessage = "";
    }

    // 3. Tangani Submit pendaftaran (Enter)
    if (IsKeyPressed(KEY_ENTER) && inputLen > 0) {
        std::string username(inputBuffer);
        PlayerProfile dummyProfile;
        
        // Cek apakah username sudah ada di database JSON
        if (DataManager::getInstance().FindPlayer(username, dummyProfile)) {
            statusMessage = "Username already taken!";
            statusColor = RED;
        } else {
            // Buat akun baru dengan nilai default di JSON
            DataManager::getInstance().CreatePlayer(username);
            
            registeredUsername = username;
            isRegisterSuccess = true;
            statusMessage = "Registration Successful!";
            statusColor = GREEN;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        goToLogin = true;
    }
}

void RegisterScreen::Draw() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    int titleFontSize = 50;
    const char* titleText = "REGISTER";
    int titleWidth = MeasureText(titleText, titleFontSize);
    DrawText(titleText, (screenWidth - titleWidth) / 2, screenHeight / 2 - 150, titleFontSize, WHITE);

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

    // Teks Input User
    int inputFontSize = 24;
    int inputWidth = MeasureText(inputBuffer, inputFontSize);
    DrawText(inputBuffer, boxX + (boxWidth - inputWidth) / 2, boxY + (boxHeight - inputFontSize) / 2, inputFontSize, WHITE);

    // Pesan Status 
    if (!statusMessage.empty()) {
        int statusWidth = MeasureText(statusMessage.c_str(), 16);
        DrawText(statusMessage.c_str(), (screenWidth - statusWidth) / 2, boxY + boxHeight + 15, 16, statusColor);
    }

    // Hint
    const char* hint1 = "Press ENTER to register";
    const char* hint2 = "[ESC] Back to Login";
    
    DrawText(hint1, (screenWidth - MeasureText(hint1, 16)) / 2, boxY + boxHeight + 70, 16, GRAY);
    DrawText(hint2, (screenWidth - MeasureText(hint2, 16)) / 2, boxY + boxHeight + 100, 16, LIGHTGRAY);
}

bool RegisterScreen::IsRegisterSuccess() const { return isRegisterSuccess; }
bool RegisterScreen::ShouldGoToLogin() const { return goToLogin; }
const std::string& RegisterScreen::GetRegisteredUsername() const { return registeredUsername; }