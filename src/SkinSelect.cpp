#include "SkinSelect.h"
#include "SkinManager.h"
#include "raylib.h"
#include <cmath>

SkinSelect::SkinSelect() {
    cursorRow = 0;
    cursorCol = 0;
    skinChanged = false;
}

SkinSelect::~SkinSelect() {}

Rectangle SkinSelect::getCellRect(int col, int row) const {
    float startX = 1080.0f / 2 - (GRID_COLS * 140 + 40) / 2.0f;
    float startY = 220.0f;
    float cellW = 140.0f;
    float cellH = 130.0f;
    float gap = 10.0f;
    return { startX + col * (cellW + gap), startY + row * (cellH + gap), cellW, cellH };
}

Rectangle SkinSelect::getShipFrame(int col, int row) const {
    const auto& tex = SkinManager::getInstance().getSpritesheet();
    float fw = (float)tex.width / GRID_COLS;
    float fh = (float)tex.height / GRID_ROWS;
    return { col * fw, row * fh, fw, fh };
}

// Gambar isi cell: ship preview jika unlocked, atau icon lock jika belum
void SkinSelect::drawCellVisual(int col, int row, Rectangle cell, bool unlocked, bool isEquipped) {
    if (unlocked) {
        const auto& tex = SkinManager::getInstance().getSpritesheet();
        Rectangle src = getShipFrame(col, row);
        float scale = (cell.width * 0.5f) / src.width;
        float destW = src.width * scale;
        float destH = src.height * scale;
        Rectangle dest = { cell.x + cell.width / 2, cell.y + cell.height / 2 - 5, destW, destH };
        Vector2 origin = { destW / 2, destH / 2 };
        DrawTexturePro(tex, src, dest, origin, 0.0f, WHITE);
        if (isEquipped) {
            DrawText("EQUIPPED", cell.x + cell.width / 2 - MeasureText("EQUIPPED", 12) / 2, cell.y + 5, 12, GOLD);
        }
    } else {
        DrawText("?", cell.x + cell.width / 2 - 15, cell.y + cell.height / 2 - 25, 30, ColorAlpha(WHITE, 0.3f));
        DrawText("LOCKED", cell.x + 20, cell.y + cell.height / 2 + 5, 14, ColorAlpha(WHITE, 0.3f));
    }
}

void SkinSelect::drawSkinCell(int col, int row, bool unlocked, bool isEquipped, bool isSelected) {
    Rectangle cell = getCellRect(col, row);
    int skinId = row * GRID_COLS + col;
    const SkinInfo& info = getSkinInfo(skinId);

    Color bgColor = unlocked ? ColorAlpha(info.glowColor, 0.15f) : ColorAlpha(DARKGRAY, 0.4f);
    Color borderColor = isSelected ? (isEquipped ? GOLD : YELLOW) : (unlocked ? ColorAlpha(info.glowColor, 0.5f) : DARKGRAY);

    DrawRectangleRoundedLines(cell, 0.1f, 8, borderColor);
    DrawRectangleRounded(cell, 0.1f, 8, bgColor);

    drawCellVisual(col, row, cell, unlocked, isEquipped);

    DrawText(info.name.c_str(), cell.x + 5, cell.y + cell.height - 22, 14, unlocked ? WHITE : ColorAlpha(WHITE, 0.3f));
}

// Navigasi grid dengan arrow keys, wrap-around di tepi
void SkinSelect::handleNavigation() {
    if (IsKeyPressed(KEY_LEFT)) {
        cursorCol--;
        if (cursorCol < 0) cursorCol = GRID_COLS - 1;
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        cursorCol++;
        if (cursorCol >= GRID_COLS) cursorCol = 0;
    }
    if (IsKeyPressed(KEY_UP)) {
        cursorRow--;
        if (cursorRow < 0) cursorRow = GRID_ROWS - 1;
    }
    if (IsKeyPressed(KEY_DOWN)) {
        cursorRow++;
        if (cursorRow >= GRID_ROWS) cursorRow = 0;
    }
}

// ENTER: equip skin yang sudah dimiliki, atau beli jika belum
void SkinSelect::handleSelection() {
    int skinId = cursorRow * GRID_COLS + cursorCol;
    auto& mgr = SkinManager::getInstance();

    if (IsKeyPressed(KEY_ENTER)) {
        if (mgr.isUnlocked(skinId)) {
            mgr.setActiveSkin(skinId);
            mgr.save();
            skinChanged = true;
        } else {
            const SkinInfo& info = getSkinInfo(skinId);
            if (mgr.spendRP(info.price)) {
                mgr.unlockSkin(skinId);
                mgr.setActiveSkin(skinId);
                mgr.save();
                skinChanged = true;
            }
        }
    }
}

// G: buka gacha, ESC: kembali ke menu
void SkinSelect::handleSpecialKeys(bool& backToMenu, bool& goToGacha) {
    if (IsKeyPressed(KEY_G)) {
        goToGacha = true;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        backToMenu = true;
    }
}

void SkinSelect::Update(bool& backToMenu, bool& goToGacha) {
    backToMenu = false;
    goToGacha = false;

    handleNavigation();
    handleSelection();
    handleSpecialKeys(backToMenu, goToGacha);
}

// Render header: title, guide, RP counter, skin count
void SkinSelect::drawHeader() {
    auto& mgr = SkinManager::getInstance();
    DrawText("SKIN BAY", 1080 / 2 - MeasureText("SKIN BAY", 50) / 2, 60, 50, WHITE);

    const char* guide = "Arrow Keys: Navigate  |  ENTER: Equip / Buy  |  G: Gacha  |  ESC: Back";
    DrawText(guide, 1080 / 2 - MeasureText(guide, 14) / 2, 120, 14, ColorAlpha(WHITE, 0.5f));

    char rpText[32];
    snprintf(rpText, sizeof(rpText), "RP: %d", mgr.getRP());
    DrawText(rpText, 1080 - MeasureText(rpText, 24) - 20, 65, 24, GOLD);

    int unlocked = 0;
    for (int i = 0; i < SKIN_COUNT; i++) {
        if (mgr.isUnlocked(i)) unlocked++;
    }
    const char* skinCountText = TextFormat("Skins: %d/15", unlocked);
    DrawText(skinCountText, 1080 - MeasureText(skinCountText, 18) - 20, 100, 18, ColorAlpha(WHITE, 0.6f));
}

// Render grid skin dengan navigasi cursor
void SkinSelect::drawGrid() {
    auto& mgr = SkinManager::getInstance();
    int activeSkin = mgr.getActiveSkin();

    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            int sid = r * GRID_COLS + c;
            bool owned = mgr.isUnlocked(sid);
            bool equipped = (sid == activeSkin);
            bool selected = (r == cursorRow && c == cursorCol);
            drawSkinCell(c, r, owned, equipped, selected);
        }
    }
}

// Tampilkan detail skin yang dipilih (owned): nama, rarity, equipped status
void SkinSelect::drawOwnedDetail(int skinId, const SkinInfo& info, int activeSkin) {
    char detailText[128];
    snprintf(detailText, sizeof(detailText), "%s  |  %s", info.name.c_str(), rarityLabel(info.rarity));
    DrawText(detailText, 1080 / 2 - MeasureText(detailText, 20) / 2, 640, 20, info.glowColor);

    if (skinId == activeSkin) {
        const char* eqMsg = "[ EQUIPPED ]";
        DrawText(eqMsg, 1080 / 2 - MeasureText(eqMsg, 16) / 2, 670, 16, GOLD);
    }
}

// Tampilkan detail skin yang belum dimiliki: nama, rarity, harga, dan action hint
void SkinSelect::drawUnownedDetail(const SkinInfo& info) {
    auto& mgr = SkinManager::getInstance();
    char detailText[128];
    snprintf(detailText, sizeof(detailText), "%s  |  %s  |  Buy: %d RP", info.name.c_str(), rarityLabel(info.rarity), info.price);
    Color buyColor = (mgr.getRP() >= info.price) ? GREEN : RED;
    DrawText(detailText, 1080 / 2 - MeasureText(detailText, 20) / 2, 640, 20, buyColor);

    if (mgr.getRP() >= info.price) {
        const char* buyMsg = "[ ENTER to Buy ]";
        DrawText(buyMsg, 1080 / 2 - MeasureText(buyMsg, 16) / 2, 670, 16, YELLOW);
    } else {
        const char* poorMsg = "[ Not enough RP ]";
        DrawText(poorMsg, 1080 / 2 - MeasureText(poorMsg, 16) / 2, 670, 16, RED);
    }
}

// Render panel detail untuk skin yang sedang dipilih cursor
void SkinSelect::drawDetailPanel() {
    auto& mgr = SkinManager::getInstance();
    int activeSkin = mgr.getActiveSkin();
    int skinId = cursorRow * GRID_COLS + cursorCol;
    const SkinInfo& info = getSkinInfo(skinId);
    bool owned = mgr.isUnlocked(skinId);

    if (owned) {
        drawOwnedDetail(skinId, info, activeSkin);
    } else {
        drawUnownedDetail(info);
    }
}

void SkinSelect::Draw() {
    drawHeader();
    drawGrid();
    drawDetailPanel();
}
