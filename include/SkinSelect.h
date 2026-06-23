#pragma once
#include "raylib.h"
#include "SkinData.h"

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
    void drawCellVisual(int col, int row, Rectangle cell, bool unlocked, bool isEquipped); // Gambar ship atau icon lock
    void handleNavigation();     // Arrow keys untuk navigasi grid
    void handleSelection();      // ENTER untuk equip/buy
    void handleSpecialKeys(bool& backToMenu, bool& goToGacha); // G untuk gacha, ESC untuk back
    void drawHeader();           // Title, guide, RP, skin count
    void drawGrid();             // Loop render grid skin
    void drawDetailPanel();      // Info skin yang dipilih di bagian bawah
    void drawOwnedDetail(int skinId, const SkinInfo& info, int activeSkin);
    void drawUnownedDetail(const SkinInfo& info);
};
