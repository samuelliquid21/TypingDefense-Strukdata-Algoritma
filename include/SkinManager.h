#pragma once
#include "SkinData.h"
#include <vector>

class SkinManager {
public:
    static SkinManager& getInstance();

    void init();
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

private:
    SkinManager() = default;
    SkinManager(const SkinManager&) = delete;
    SkinManager& operator=(const SkinManager&) = delete;

    int researchPoint = 300;
    int activeSkin = 0;
    std::vector<int> unlockedSkins;
    Texture2D spritesheet{0};

public:
    const Texture2D& getSpritesheet() const { return spritesheet; }
};
