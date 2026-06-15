#pragma once
#include "raylib.h"
#include "SkinData.h"
#include <vector>

class GachaScreen {
public:
    GachaScreen();
    ~GachaScreen();

    void Update(bool& backToSkinSelect, bool& goToSkinSelect);
    void Draw();

private:
    enum Phase {
        IDLE,
        SPINNING,
        LANDED,
        RESULT,
        ALL_COLLECTED
    };

    Phase phase;
    bool contentLoaded;

    // Carousel item
    struct CarouselItem {
        GachaItemType type;
        int id;
        int rarity;
        const char* name;
        Color color;
    };
    std::vector<CarouselItem> pool;
    static const int POOL_SIZE = 24;

    // Spin animation
    float scrollOffset;
    float startOffset;
    float targetOffset;
    float spinTimer;
    float spinDuration;

    // Reward
    GachaItemType rewardType;
    int rewardId;
    int rewardRarity;

    // Display
    float resultTimer;
    bool freeSpinUsed;
    bool rewardAlreadyOwned;
    int rewardZonkVariant;

    // Textures (lazy loaded)
    Texture2D texZonk1, texZonk2, texZonk3, texZonk4, texCoin, texFreeSpin;
    bool texLoaded;

    // Sounds (lazy loaded)
    Sound sndSpinWheels, sndZonk, sndCoin, sndFreeSpin;
    Sound sndCommon, sndRare, sndEpic, sndLegendary;
    bool sndLoaded;
    bool spinSoundPlaying;

    void BuildPool();
    void LoadContent();
    void UnloadContent();
    void StartSpin();
    void UpdateSpin(float dt);
    void SelectReward();
    void DrawCarousel();
    void DrawSlot(int poolIdx, float x, float cy, float w, float h, float alpha, bool highlight);
    void DrawResultScreen();
    void PlayRewardSound();
    Texture2D* GetBonusTexture(int poolIdx);
};
