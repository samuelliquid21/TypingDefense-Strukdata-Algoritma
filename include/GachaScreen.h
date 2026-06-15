#pragma once
#include "raylib.h"

enum class GachaPhase {
    IDLE,
    SHAKING,
    OPENING,
    FLIPPING,
    RESULT,
    ALL_COLLECTED
};

class GachaScreen {
public:
    GachaScreen();
    ~GachaScreen();

    void Update(bool& backToSkinSelect, bool& goToSkinSelect);
    void Draw();

private:
    GachaPhase phase;
    float phaseTimer;
    int resultSkinId;
    float shakeOffset;
    float flipProgress;
    float resultDisplayTimer;

    void startPull();
};
