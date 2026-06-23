#include "Pause.h"
#include <string>

// ===============================
// ⏸️ MENU PAUSE
// ===============================

PauseMenu::PauseMenu() {
    title = "PAUSED";
    options = { "Resume Game", "Back to Menu" };
    selectedIndex = 0;
    optionChosen = false;
    titleFontSize = 80;
    menuFontSize = 30;

    isCountingDown = false;
    countdownTimer = 0.0f;
}

// Navigasi opsi naik/turun dengan wrap-around, ENTER untuk memilih
void PauseMenu::handleNavigation() {
    if (IsKeyPressed(KEY_DOWN)) {
        selectedIndex++;
        if (selectedIndex >= options.size()) selectedIndex = 0;
    }
    if (IsKeyPressed(KEY_UP)) {
        selectedIndex--;
        if (selectedIndex < 0) selectedIndex = options.size() - 1;
    }
    if (IsKeyPressed(KEY_ENTER)) {
        optionChosen = true;
    }
}

// Update navigasi menu atau countdown sebelum resume
void PauseMenu::Update() {
    if (isCountingDown) {
        // Kurangi timer countdown setiap frame
        countdownTimer -= GetFrameTime();
    }
    else {
        handleNavigation();
    }
}

// Tampilkan angka countdown (3, 2, 1, GO!)
void PauseMenu::drawCountdown() {
    int currentNumber = (int)countdownTimer;
    if (currentNumber > 0) {
        std::string countText = std::to_string(currentNumber);
        int textWidth = MeasureText(countText.c_str(), 120);
        DrawText(countText.c_str(), (1080 - textWidth) / 2, (720 - 120) / 2, 120, WHITE);
    } else if (currentNumber == 0) {
        std::string countText = "GO!";
        int textWidth = MeasureText(countText.c_str(), 120);
        DrawText(countText.c_str(), (1080 - textWidth) / 2, (720 - 120) / 2, 120, GREEN);
    }
}

// Tampilan menu pause dengan judul dan pilihan
void PauseMenu::drawMenuOptions() {
    int titleWidth = MeasureText(title.c_str(), titleFontSize);
    DrawText(title.c_str(), (1080 - titleWidth) / 2, 200, titleFontSize, WHITE);

    for (int i = 0; i < options.size(); i++) {
        std::string text = options[i];
        int textWidth = MeasureText(text.c_str(), menuFontSize);
        int x = (1080 - textWidth) / 2;
        int y = 350 + (i * 60);

        Color color = (i == selectedIndex) ? YELLOW : WHITE;
        DrawText(text.c_str(), x, y, menuFontSize, color);
    }
}

// Render overlay pause: background gelap, menu atau countdown
void PauseMenu::Draw() {
    // Overlay transparan di belakang
    DrawRectangle(0, 0, 1080, 720, Fade(BLACK, 0.6f));

    if (isCountingDown) {
        drawCountdown();
    }
    else {
        drawMenuOptions();
    }
}
