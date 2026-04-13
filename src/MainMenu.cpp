#include "MainMenu.h"

MainMenu::MainMenu() {
    title = "Cosmic Keypad";

    options = {
        "Play",
        "Leaderboard",
        "Credit",
        "Quit"
    };

    selectedIndex = 0;
    optionChosen = false;

    titleFontSize = 100;
    menuFontSize = 30;
}

void MainMenu::Update() {
    if (IsKeyPressed(KEY_DOWN)) {
        selectedIndex++;
        if (selectedIndex >= options.size())
            selectedIndex = 0;
    }

    if (IsKeyPressed(KEY_UP)) {
        selectedIndex--;
        if (selectedIndex < 0)
            selectedIndex = options.size() - 1;
    }

    if (IsKeyPressed(KEY_ENTER)) {
        optionChosen = true;
    }
}

void MainMenu::Draw() {
    // Draw Title
    int titleWidth = MeasureText(title.c_str(), titleFontSize);
    DrawText(
        title.c_str(),
        (1080 - titleWidth) / 2,
        100,
        titleFontSize,
        WHITE
    );

    // Draw Menu Options
    for (int i = 0; i < options.size(); i++) {
        std::string text = options[i];
        int textWidth = MeasureText(text.c_str(), menuFontSize);

        int x = (1080 - textWidth) / 2;
        int y = 450 + i * 60;

        if (i == selectedIndex)
            DrawText(text.c_str(), x, y, menuFontSize, YELLOW);
        else
            DrawText(text.c_str(), x, y, menuFontSize, WHITE);
    }
}

int MainMenu::GetSelectedIndex() const {
    return selectedIndex;
}

bool MainMenu::IsOptionChosen() const {
    return optionChosen;
}

void MainMenu::ResetChoice() {
    optionChosen = false;
}