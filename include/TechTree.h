#pragma once

#include <raylib.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <queue>
#include "DataManager.h"

enum SkillName {
    BARRIER,
    AURA_FIELD,
    SHOCKWAVE,
    CHRONO_STASIS,
    INSTANT_CRIT,
    SCORE_BOOSTER
};

enum SkillState {
    LOCKED,      // Tidak bisa diakses (Abu-abu)
    AVAILABLE,   // Bisa dibeli/di-unlock (Kuning/Biru)
    UNLOCKED     // Sudah aktif (Hijau)
};

struct SkillData {
    SkillName name;
    int rpCost;
    std::string desc;
    bool isUnlocked;

    // UI Data untuk Raylib
    Vector2 position;     // Titik tengah node untuk menggambar garis antar skill
    Rectangle bounds;     // Area hitbox untuk mendeteksi klik/hover mouse
    SkillState uiState;   // Status untuk mempermudah pewarnaan di Raylib
};

class TechTree {
private:
    std::unordered_map<SkillName, SkillData> skills;
    std::unordered_map<SkillName, std::vector<SkillName>> adjList;

    void updateSkillStates();

public:
    TechTree();

    void addDependency(SkillName parent, SkillName child);

    static SkillName SkillNameFromString(const std::string& str);
    static std::string StringFromSkillName(SkillName name);

    void loadFromProfile(const PlayerProfile& profile);
    void saveToProfile(PlayerProfile& profile);

    bool unlockSkill(SkillName name, PlayerProfile& profile);

    bool handleInput(Vector2 mousePos, bool isMouseClicked, PlayerProfile& profile);

    const std::unordered_map<SkillName, SkillData>& getSkills() const;
    const std::unordered_map<SkillName, std::vector<SkillName>>& getAdjList() const;
};
