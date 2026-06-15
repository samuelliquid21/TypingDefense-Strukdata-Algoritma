#include "SkinManager.h"
#include "DataManager.h"
#include "raylib.h"
#include <algorithm>

SkinManager& SkinManager::getInstance() {
    static SkinManager instance;
    return instance;
}

void SkinManager::init() {
    auto& dm = DataManager::getInstance();
    dm.load();

    auto& data = dm.getData();

    if (!data.is_null() && data.contains("data") && !data["data"].empty()) {
        auto& player = data["data"][0];

        if (player.contains("research_point"))
            researchPoint = player["research_point"];

        if (player.contains("free_spin"))
            freeSpinCount = player["free_spin"];

        if (player.contains("active_skin"))
            activeSkin = player["active_skin"];

        unlockedSkins.clear();
        if (player.contains("unlocked_skins") && player["unlocked_skins"].is_array()) {
            for (auto& s : player["unlocked_skins"])
                unlockedSkins.push_back(s);
        }
    }

    if (unlockedSkins.empty()) {
        unlockedSkins = {0, 5, 10};
    }

    if (activeSkin == 0 && !unlockedSkins.empty())
        activeSkin = unlockedSkins[0];

    spritesheet = LoadTexture("assets/img/Spaceships.png");
}

void SkinManager::save() {
    auto& dm = DataManager::getInstance();
    auto& data = dm.getData();

    if (data.is_null() || !data.contains("data") || data["data"].empty()) {
        data["data"] = json::array();
        data["data"][0] = json::object();
    }

    auto& player = data["data"][0];
    player["research_point"] = researchPoint;
    player["free_spin"] = freeSpinCount;
    player["active_skin"] = activeSkin;

    json arr = json::array();
    for (int id : unlockedSkins) arr.push_back(id);
    player["unlocked_skins"] = arr;

    dm.save();
}

int SkinManager::getRP() const { return researchPoint; }

void SkinManager::addRP(int amount) {
    researchPoint += amount;
}

bool SkinManager::spendRP(int amount) {
    if (researchPoint < amount) return false;
    researchPoint -= amount;
    return true;
}

bool SkinManager::isUnlocked(int skinId) const {
    return std::find(unlockedSkins.begin(), unlockedSkins.end(), skinId) != unlockedSkins.end();
}

void SkinManager::unlockSkin(int skinId) {
    if (!isUnlocked(skinId)) {
        unlockedSkins.push_back(skinId);
    }
}

int SkinManager::getActiveSkin() const { return activeSkin; }

void SkinManager::setActiveSkin(int skinId) {
    if (isUnlocked(skinId)) activeSkin = skinId;
}

const std::vector<int>& SkinManager::getUnlockedSkins() const {
    return unlockedSkins;
}

int SkinManager::gachaPull() {
    std::vector<int> unowned;
    for (int id = 1; id < SKIN_COUNT; id++) {
        if (id != 5 && id != 10 && !isUnlocked(id))
            unowned.push_back(id);
    }

    if (unowned.empty()) return -1;

    int weightsByCol[5] = {0, 45, 30, 17, 8};

    int totalWeight = 0;
    for (int id : unowned) {
        totalWeight += weightsByCol[skinToCol(id)];
    }

    int roll = GetRandomValue(1, totalWeight);
    int cumulative = 0;
    for (int id : unowned) {
        cumulative += weightsByCol[skinToCol(id)];
        if (roll <= cumulative) return id;
    }

    return unowned.back();
}
