#pragma once
#include "SkinData.h"
#include <vector>

class SkinManager {
public:
    static SkinManager& getInstance();

    void init();
    void init(const std::string& username);
    void save();

    int getRP() const;
    void addRP(int amount);
    bool spendRP(int amount);

    bool isUnlocked(int skinId) const;
    void unlockSkin(int skinId);

    int getActiveSkin() const;
    void setActiveSkin(int skinId);

    const std::vector<int>& getUnlockedSkins() const;

    int gachaPull();

    int getFreeSpinCount() const { return freeSpinCount; }
    void addFreeSpin(int amount) { freeSpinCount += amount; }
    bool useFreeSpin() {
        if (freeSpinCount <= 0) return false;
        freeSpinCount--;
        return true;
    }
    void setFreeSpinCount(int count) { freeSpinCount = count; }

private:
    SkinManager() = default;
    SkinManager(const SkinManager&) = delete;
    SkinManager& operator=(const SkinManager&) = delete;

    std::string currentUsername;
    int researchPoint = 300;
    int activeSkin = 0;
    int freeSpinCount = 0;
    std::vector<int> unlockedSkins;
    Texture2D spritesheet{0};

    void parsePlayerData(const std::string& username); // Ekstrak data player dari JSON ke member
    void createNewPlayerEntry();  // Buat entry player baru di JSON lalu simpan
    int weightedRandomSelect(const std::vector<int>& ids) const; // Pilih ID berdasarkan bobot rarity

public:
    const Texture2D& getSpritesheet() const { return spritesheet; }
};
