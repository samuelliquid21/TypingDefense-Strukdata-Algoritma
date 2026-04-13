#pragma once
#include "raylib.h"
#include <vector>
#include <string>

class MainMenu {
public:
    MainMenu();

    void Update();
    void Draw();

    int GetSelectedIndex() const;
    bool IsOptionChosen() const;
    void ResetChoice();

private:
    std::string title;
    std::vector<std::string> options;

    int selectedIndex;
    bool optionChosen;

    int titleFontSize;
    int menuFontSize;
};