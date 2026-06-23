#include "LoginScreen.h"

// ===============================
// 🔐 LAYAR LOGIN
// ===============================

LoginScreen::LoginScreen() {
    Reset();
}

// Reset semua input dan status ke nilai awal
void LoginScreen::Reset() {
    inputBuffer[0] = '\0';
    inputLen = 0;
    statusMessage = "";
    shouldLogin = false;
    goToRegister = false;
    goBack = false;
}

// Baca karakter yang diketik dari keyboard
void LoginScreen::handleTextInput() {
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 125 && inputLen < 31) {
            inputBuffer[inputLen] = (char)key;
            inputLen++;
            inputBuffer[inputLen] = '\0';
        }
        key = GetCharPressed();
    }
}

// Hapus karakter terakhir dengan BACKSPACE
void LoginScreen::handleBackspace() {
    if (IsKeyPressed(KEY_BACKSPACE) && inputLen > 0) {
        inputLen--;
        inputBuffer[inputLen] = '\0';
        statusMessage = "";
    }
}

// Submit login dengan ENTER: cari player di database
void LoginScreen::handleSubmit() {
    if (IsKeyPressed(KEY_ENTER) && inputLen > 0) {
        std::string username(inputBuffer);
        PlayerProfile profile;

        if (DataManager::getInstance().FindPlayer(username, profile)) {
            loggedInProfile = profile;
            shouldLogin = true;
        } else {
            statusMessage = "Username not found!";
        }
    }
}

// TAB → pindah ke layar register, ESC → keluar game
void LoginScreen::handleNavigationKeys() {
    if (IsKeyPressed(KEY_TAB)) {
        goToRegister = true;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        goBack = true;
    }
}

// Tangani input keyboard: ketik username, hapus, submit ENTER, TAB, ESC
void LoginScreen::Update() {
    handleTextInput();
    handleBackspace();
    handleSubmit();
    handleNavigationKeys();
}

// Render judul "LOGIN" di tengah layar
void LoginScreen::drawLoginTitle() {
    int screenWidth = GetScreenWidth();
    int titleFontSize = 50;
    const char* titleText = "LOGIN";
    int titleWidth = MeasureText(titleText, titleFontSize);
    DrawText(titleText, (screenWidth - titleWidth) / 2, GetScreenHeight() / 2 - 150, titleFontSize, WHITE);
}

// Render label "Enter username:" di atas input box
void LoginScreen::drawLoginLabel() {
    int screenWidth = GetScreenWidth();
    int labelFontSize = 20;
    const char* labelText = "Enter username:";
    int labelWidth = MeasureText(labelText, labelFontSize);
    DrawText(labelText, (screenWidth - labelWidth) / 2, GetScreenHeight() / 2 - 60, labelFontSize, LIGHTGRAY);
}

// Render kotak input dan teks yang diketik user
void LoginScreen::drawLoginInputBox() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int boxWidth = 400;
    int boxHeight = 50;
    int boxX = (screenWidth - boxWidth) / 2;
    int boxY = screenHeight / 2 - 30;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(DARKGRAY, 0.8f));
    DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, WHITE);

    int inputFontSize = 24;
    int inputWidth = MeasureText(inputBuffer, inputFontSize);
    DrawText(inputBuffer, boxX + (boxWidth - inputWidth) / 2, boxY + (boxHeight - inputFontSize) / 2, inputFontSize, WHITE);
}

// Render judul, label, kotak input
void LoginScreen::drawLoginForm() {
    drawLoginTitle();
    drawLoginLabel();
    drawLoginInputBox();
}

// Render pesan status (error)
void LoginScreen::drawStatusMessage() {
    if (statusMessage.empty()) return;

    int screenWidth = GetScreenWidth();
    int boxWidth = 400;
    int boxHeight = 50;
    int boxY = GetScreenHeight() / 2 - 30;

    int statusWidth = MeasureText(statusMessage.c_str(), 16);
    DrawText(statusMessage.c_str(), (screenWidth - statusWidth) / 2, boxY + boxHeight + 15, 16, RED);
}

// Render hint navigasi di bawah form
void LoginScreen::drawNavigationHints() {
    int screenWidth = GetScreenWidth();
    int boxWidth = 400;
    int boxHeight = 50;
    int boxY = GetScreenHeight() / 2 - 30;

    const char* hint1 = "Press ENTER to login";
    const char* hint2 = "[TAB] Register new account   |   [ESC] Quit";

    DrawText(hint1, (screenWidth - MeasureText(hint1, 16)) / 2, boxY + boxHeight + 70, 16, GRAY);
    DrawText(hint2, (screenWidth - MeasureText(hint2, 16)) / 2, boxY + boxHeight + 100, 16, LIGHTGRAY);
}

// Render form login: judul, label, kotak input, hint
void LoginScreen::Draw() {
    drawLoginForm();
    drawStatusMessage();
    drawNavigationHints();
}

bool LoginScreen::ShouldLogin() const { return shouldLogin; }
bool LoginScreen::ShouldGoToRegister() const { return goToRegister; }
bool LoginScreen::ShouldGoBack() const { return goBack; }
const PlayerProfile& LoginScreen::GetProfile() const { return loggedInProfile; }
