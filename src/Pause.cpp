#include "Pause.h"
#include <string>

PauseMenu::PauseMenu() {
    title = "PAUSED";
    options = { "Resume Game", "Back to Menu" };
    selectedIndex = 0;
    optionChosen = false;
    titleFontSize = 80;
    menuFontSize = 30;

    isCountingDown = false;   // Awalnya tidak dalam countdown
    countdownTimer = 0.0f;
}

void PauseMenu::Update() {
    if (isCountingDown) {
        // Kurangi timer countdown setiap frame
        countdownTimer -= GetFrameTime(); 
    } 
    else {
        // Navigasi opsi dengan UP/DOWN, wrap-around
        if (IsKeyPressed(KEY_DOWN)) {
            selectedIndex++;
            if (selectedIndex >= options.size()) selectedIndex = 0;
        }

        if (IsKeyPressed(KEY_UP)) {
            selectedIndex--;
            if (selectedIndex < 0) selectedIndex = options.size() - 1;
        }

        // ENTER untuk memilih opsi
        if (IsKeyPressed(KEY_ENTER)) {
            optionChosen = true;
        }
    }
}

void PauseMenu::Draw() {
    // Overlay hitam transparan di belakang untuk efek "pause"
    DrawRectangle(0, 0, 1080, 720, Fade(BLACK, 0.6f));

    if (isCountingDown) {
        // Tampilkan angka countdown besar di tengah layar
        int currentNumber = (int)countdownTimer; 
        
        if (currentNumber > 0) {
            // Tampilkan angka 3, 2, 1
            std::string countText = std::to_string(currentNumber);
            int textWidth = MeasureText(countText.c_str(), 120);
            
            DrawText(countText.c_str(), (1080 - textWidth) / 2, (720 - 120) / 2, 120, WHITE);
        } else if (currentNumber == 0) {
            // Setelah angka habis, tampilkan "GO!" warna hijau
            std::string countText = "GO!";
            int textWidth = MeasureText(countText.c_str(), 120);
            
            DrawText(countText.c_str(), (1080 - textWidth) / 2, (720 - 120) / 2, 120, GREEN);
        }
    } 
    else {
        // Tampilan menu pause dengan daftar opsi
        int titleWidth = MeasureText(title.c_str(), titleFontSize);
        DrawText(title.c_str(), (1080 - titleWidth) / 2, 200, titleFontSize, WHITE);

        for (int i = 0; i < options.size(); i++) {
            std::string text = options[i];
            int textWidth = MeasureText(text.c_str(), menuFontSize);
            int x = (1080 - textWidth) / 2;
            int y = 350 + (i * 60);

            // Option yang terpilih warna YELLOW, sisanya WHITE
            Color color = (i == selectedIndex) ? YELLOW : WHITE;

            if (i == selectedIndex) {
                DrawText((text).c_str(), x, y, menuFontSize, color);
            } else {
                DrawText(text.c_str(), x, y, menuFontSize, color);
            }
        }
    }
}
