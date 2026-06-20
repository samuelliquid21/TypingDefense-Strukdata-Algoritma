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
    // Untuk menangani input ketikan keyboard
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
        statusMessage = ""; // Hapus pesan error jika pemain mengetik ulang
    }

    if (IsKeyPressed(KEY_ENTER) && inputLen > 0) {
        std::string username(inputBuffer);
        PlayerProfile profile;
        
        // Cari data pemain di database lokal
        if (DataManager::getInstance().FindPlayer(username, profile)) {
            loggedInProfile = profile;
            shouldLogin = true;
        } else {
            statusMessage = "Username not found!";
        }
    }

    if (IsKeyPressed(KEY_TAB)) {
        goToRegister = true;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        goBack = true;
    }
}

void LoginScreen::Draw() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    int titleFontSize = 50;
    const char* titleText = "LOGIN";
    int titleWidth = MeasureText(titleText, titleFontSize);
    DrawText(titleText, (screenWidth - titleWidth) / 2, screenHeight / 2 - 150, titleFontSize, WHITE);

    int labelFontSize = 20;
    const char* labelText = "Enter username:";
    int labelWidth = MeasureText(labelText, labelFontSize);
    DrawText(labelText, (screenWidth - labelWidth) / 2, screenHeight / 2 - 60, labelFontSize, LIGHTGRAY);

    // Kotak Input
    int boxWidth = 400;
    int boxHeight = 50;
    int boxX = (screenWidth - boxWidth) / 2;
    int boxY = screenHeight / 2 - 30;
    
    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(DARKGRAY, 0.8f));
    DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, WHITE); // Garis luar agar tegas

    // Teks input user
    int inputFontSize = 24;
    int inputWidth = MeasureText(inputBuffer, inputFontSize);
    DrawText(inputBuffer, boxX + (boxWidth - inputWidth) / 2, boxY + (boxHeight - inputFontSize) / 2, inputFontSize, WHITE);

    // Pesan status
    if (!statusMessage.empty()) {
        int statusWidth = MeasureText(statusMessage.c_str(), 16);
        DrawText(statusMessage.c_str(), (screenWidth - statusWidth) / 2, boxY + boxHeight + 15, 16, RED);
    }

    // Hint
    const char* hint1 = "Press ENTER to login";
    const char* hint2 = "[TAB] Register new account   |   [ESC] Quit";
    
    DrawText(hint1, (screenWidth - MeasureText(hint1, 16)) / 2, boxY + boxHeight + 70, 16, GRAY);
    DrawText(hint2, (screenWidth - MeasureText(hint2, 16)) / 2, boxY + boxHeight + 100, 16, LIGHTGRAY);
}

bool LoginScreen::ShouldLogin() const { return shouldLogin; }
bool LoginScreen::ShouldGoToRegister() const { return goToRegister; }
bool LoginScreen::ShouldGoBack() const { return goBack; }
const PlayerProfile& LoginScreen::GetProfile() const { return loggedInProfile; }