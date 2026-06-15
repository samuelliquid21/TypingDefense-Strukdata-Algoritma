#pragma once
#include "raylib.h"

class SkinSelect {
public:
    SkinSelect();
    ~SkinSelect();

    void Update(bool& backToMenu, bool& goToGacha);
    void Draw();

private:
    int cursorRow;
    int cursorCol;
    bool skinChanged;

    Rectangle getCellRect(int col, int row) const;
    Rectangle getShipFrame(int col, int row) const;
    void drawSkinCell(int col, int row, bool unlocked, bool isEquipped, bool isSelected);
};
