#include "SkinManager.h"
#include "DataManager.h"
#include "raylib.h"
#include <algorithm>

SkinManager& SkinManager::getInstance() {
    static SkinManager instance;
    return instance;
}

void SkinManager::init() {
    currentUsername.clear();
    researchPoint = 300;
    activeSkin = 0;
    freeSpinCount = 0;
    unlockedSkins = {0, 5, 10};
    spritesheet = LoadTexture("assets/img/Spaceships.png");
}

// Ekstrak data player dari JSON ke member variables
void SkinManager::parsePlayerData(const std::string& username) {
    auto& data = DataManager::getInstance().getData();
    if (data.is_null() || !data.contains("data") || !data["data"].is_array()) return;

    for (auto& player : data["data"]) {
        if (player.contains("username") && player["username"] == username) {
            if (player.contains("research_point")) researchPoint = player["research_point"];
            if (player.contains("free_spin"))      freeSpinCount = player["free_spin"];
            if (player.contains("active_skin"))    activeSkin = player["active_skin"];

            unlockedSkins.clear();
            if (player.contains("unlocked_skins") && player["unlocked_skins"].is_array()) {
                for (auto& s : player["unlocked_skins"])
                    unlockedSkins.push_back(s);
            }
            break;
        }
    }
}

void SkinManager::init(const std::string& username) {
    currentUsername = username;

    researchPoint = 0;
    activeSkin = 0;
    freeSpinCount = 0;
    unlockedSkins = {0, 5, 10};

    parsePlayerData(username);

    if (unlockedSkins.empty())
        unlockedSkins = {0, 5, 10};

    if (activeSkin == 0 && !unlockedSkins.empty())
        activeSkin = unlockedSkins[0];

    spritesheet = LoadTexture("assets/img/Spaceships.png");
}

// Buat entry player baru di JSON data, lalu simpan ke file
void SkinManager::createNewPlayerEntry() {
    auto& dm = DataManager::getInstance();
    auto& data = dm.getData();
    dm.load();
    dm.getData();
    if (data.contains("data") && data["data"].is_array()) {
        data["data"].push_back(json::object());
        auto& player = data["data"][data["data"].size()-1];
        player["username"] = currentUsername;
        player["research_point"] = researchPoint;
        player["free_spin"] = freeSpinCount;
        player["active_skin"] = activeSkin;
        json arr = json::array();
        for (int id : unlockedSkins) arr.push_back(id);
        player["unlocked_skins"] = arr;
        dm.save();
    }
}

// Update data player yang sudah ada di JSON, return true jika ditemukan
static bool updateExistingPlayerData(json& data, const std::string& username,
                                      int rp, int spin, int skin, const std::vector<int>& skins) {
    for (auto& player : data["data"]) {
        if (player.contains("username") && player["username"] == username) {
            player["research_point"] = rp;
            player["free_spin"] = spin;
            player["active_skin"] = skin;
            json arr = json::array();
            for (int id : skins) arr.push_back(id);
            player["unlocked_skins"] = arr;
            return true;
        }
    }
    return false;
}

void SkinManager::save() {
    auto& dm = DataManager::getInstance();
    auto& data = dm.getData();

    if (currentUsername.empty()) return;

    if (data.is_null() || !data.contains("data") || !data["data"].is_array()) {
        data["data"] = json::array();
    }

    if (updateExistingPlayerData(data, currentUsername, researchPoint, freeSpinCount, activeSkin, unlockedSkins)) {
        dm.save();
        return;
    }

    createNewPlayerEntry();
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

// Pilih satu ID dari daftar berdasarkan bobot rarity (weighted random selection)
int SkinManager::weightedRandomSelect(const std::vector<int>& ids) const {
    int weightsByCol[5] = {0, 45, 30, 17, 8};

    int totalWeight = 0;
    for (int id : ids) {
        totalWeight += weightsByCol[skinToCol(id)];
    }

    int roll = GetRandomValue(1, totalWeight);
    int cumulative = 0;
    for (int id : ids) {
        cumulative += weightsByCol[skinToCol(id)];
        if (roll <= cumulative) return id;
    }

    return ids.back();
}

int SkinManager::gachaPull() {
    std::vector<int> unowned;
    for (int id = 1; id < SKIN_COUNT; id++) {
        if (id != 5 && id != 10 && !isUnlocked(id))
            unowned.push_back(id);
    }

    if (unowned.empty()) return -1;

    return weightedRandomSelect(unowned);
}
