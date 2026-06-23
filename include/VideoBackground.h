#pragma once
#include "raylib.h"
#include <string>

class VideoBackground {
public:
    VideoBackground();
    ~VideoBackground();

    void Load(const std::string& frameDir);
    void Unload();
    void Update(float dt);
    void Draw(float fadeAlpha = 1.0f);
    void Restart();

    bool IsLoaded() const { return loaded; }
    int GetFrameCount() const { return totalFrames; }
    int GetCurrentFrame() const { return currentFrame; }

private:
    bool loaded;

    std::string dirPath;
    int totalFrames;
    int currentFrame;
    float frameTimer;
    float frameDelay;

    int frameW;
    int frameH;

    static const int NUM_BUFS = 2;
    Texture2D textures[NUM_BUFS];
    int displayIdx;

    std::string GetFramePath(int idx) const;
    void LoadFrameToTexture(int texIdx, int frameIdx);
};
