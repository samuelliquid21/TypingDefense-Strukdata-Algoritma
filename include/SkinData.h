#pragma once
#include "raylib.h"
#include <string>

// Bonus reward types for gacha
enum class GachaItemType {
    ZONK,
    COIN,
    FREE_SPIN,
    SKIN
};

struct GachaPoolItem {
    GachaItemType type;
    int id;          // skinId for SKIN, coin amount for COIN, 0 otherwise
    int rarity;      // 0 for ZONK, 1-5 for others
    const char* name;
    Color color;
};

struct SkinInfo {
    int id;
    std::string name;
    int rarity;
    int row;
    int col;
    int price;
    Color glowColor;
};

static const int SKIN_COUNT = 15;
static const int GRID_COLS = 5;
static const int GRID_ROWS = 3;

static const SkinInfo ALL_SKINS[SKIN_COUNT] = {
    {0,  "Green Arrow",     1, 0, 0, 0,    GREEN},
    {1,  "Silver Fang",     2, 0, 1, 150,  LIGHTGRAY},
    {2,  "Jade Serpent",    3, 0, 2, 300,  SKYBLUE},
    {3,  "Solar Storm",     4, 0, 3, 500,  PURPLE},
    {4,  "Void Sovereign",  5, 0, 4, 800,  ORANGE},

    {5,  "Red Phantom",     1, 1, 0, 0,    GREEN},
    {6,  "Rust Reaver",     2, 1, 1, 150,  LIGHTGRAY},
    {7,  "Crimson Wraith",  3, 1, 2, 300,  SKYBLUE},
    {8,  "Inferno Lord",    4, 1, 3, 500,  PURPLE},
    {9,  "Obsidian King",   5, 1, 4, 800,  ORANGE},

    {10, "Blue Stryker",    1, 2, 0, 0,    GREEN},
    {11, "Frost Hunter",    2, 2, 1, 150,  LIGHTGRAY},
    {12, "Arctic Phantom",  3, 2, 2, 300,  SKYBLUE},
    {13, "Nova Empress",    4, 2, 3, 500,  PURPLE},
    {14, "Starlight Emperor",5, 2, 4, 800, ORANGE},
};

inline const SkinInfo& getSkinInfo(int id) {
    return ALL_SKINS[id];
}

inline int skinToCol(int id) { return id % GRID_COLS; }
inline int skinToRow(int id) { return id / GRID_COLS; }

inline bool isDefaultSkin(int id) {
    return id == 0 || id == 5 || id == 10;
}

inline const char* rarityLabel(int rarity) {
    switch (rarity) {
        case 1: return "COMMON";
        case 2: return "UNCOMMON";
        case 3: return "RARE";
        case 4: return "EPIC";
        case 5: return "LEGENDARY";
        default: return "UNKNOWN";
    }
}

inline Color rarityColor(int rarity) {
    switch (rarity) {
        case 1: return GREEN;
        case 2: return LIGHTGRAY;
        case 3: return SKYBLUE;
        case 4: return PURPLE;
        case 5: return ORANGE;
        default: return WHITE;
    }
}

inline Color rarityGlow(int rarity) {
    switch (rarity) {
        case 1: return {0, 255, 65, 80};
        case 2: return {200, 200, 210, 80};
        case 3: return {0, 150, 255, 80};
        case 4: return {180, 0, 255, 80};
        case 5: return {255, 180, 0, 80};
        default: return {255, 255, 255, 40};
    }
}

// Bonus gacha item definitions
inline const char* gachaLabel(GachaItemType type, int rarity) {
    if (type == GachaItemType::SKIN) return rarityLabel(rarity);
    switch (type) {
        case GachaItemType::ZONK: return "ZONK";
        case GachaItemType::COIN: return "COIN";
        case GachaItemType::FREE_SPIN: return "FREE SPIN";
        default: return "";
    }
}

inline Color gachaColor(GachaItemType type, int rarity) {
    if (type == GachaItemType::SKIN) return rarityColor(rarity);
    switch (type) {
        case GachaItemType::ZONK: return {120, 120, 120, 255};
        case GachaItemType::COIN: return GOLD;
        case GachaItemType::FREE_SPIN: return {0, 255, 255, 255};
        default: return WHITE;
    }
}

inline Color gachaGlow(GachaItemType type, int rarity) {
    if (type == GachaItemType::SKIN) return rarityGlow(rarity);
    switch (type) {
        case GachaItemType::ZONK: return {80, 80, 80, 60};
        case GachaItemType::COIN: return {255, 215, 0, 80};
        case GachaItemType::FREE_SPIN: return {0, 255, 255, 60};
        default: return {255, 255, 255, 40};
    }
}
