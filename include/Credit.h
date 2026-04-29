#pragma once
#include "raylib.h"
#include <vector>
#include <string>

struct CreditEntry {
    std::string role;
    std::string name;
    bool isHeader;
};

class Credit {
public:
    Credit();
    void Update(bool& backToMenu);
    void Draw();

private:
    std::vector<CreditEntry> entries;
    float scrollY;
    float scrollSpeed;
    float totalHeight;
};