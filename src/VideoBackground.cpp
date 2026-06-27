#include "VideoBackground.h"
#include <string>
#include <vector>
#include <algorithm>
#include <cstdio>

VideoBackground::VideoBackground()
    : loaded(false), totalFrames(0), currentFrame(0),
      frameTimer(0.0f), frameDelay(0.083f), frameW(0), frameH(0),
      displayIdx(0) {
    textures[0] = { 0 };
    textures[1] = { 0 };
}

VideoBackground::~VideoBackground() {
    Unload();
}

std::string VideoBackground::GetFramePath(int idx) const {
    char buf[256];
    snprintf(buf, sizeof(buf), "%s/frame_%04d.png", dirPath.c_str(), idx);
    return std::string(buf);
}

void VideoBackground::LoadFrameToTexture(int texIdx, int frameIdx) {
    if (frameIdx < 0 || frameIdx >= totalFrames) return;
    std::string path = GetFramePath(frameIdx);
    Image img = LoadImage(path.c_str());
    if (img.data != nullptr) {
        if (textures[texIdx].id > 0) {
            UpdateTexture(textures[texIdx], img.data);
        }
        UnloadImage(img);
    }
}

void VideoBackground::Load(const std::string& frameDir) {
    Unload();

    dirPath = frameDir;

    // Count frames by scanning directory for frame_XXXX.png
    totalFrames = 0;
    while (true) {
        std::string path = GetFramePath(totalFrames);
        if (!FileExists(path.c_str())) break;
        totalFrames++;
    }

    if (totalFrames <= 0) {
        loaded = false;
        return;
    }

    // Load first frame to get dimensions and create textures
    std::string firstPath = GetFramePath(0);
    Image firstImg = LoadImage(firstPath.c_str());
    if (firstImg.data == nullptr) {
        loaded = false;
        return;
    }

    frameW = firstImg.width;
    frameH = firstImg.height;

    textures[0] = LoadTextureFromImage(firstImg);
    textures[1] = LoadTextureFromImage(firstImg);
    UnloadImage(firstImg);

    currentFrame = 0;
    frameTimer = 0.0f;
    displayIdx = 0;

    // Preload frame 1 into the other buffer
    if (totalFrames > 1) {
        LoadFrameToTexture(1, 1);
    }

    loaded = (textures[0].id > 0 && textures[1].id > 0);
}

void VideoBackground::Unload() {
    if (textures[0].id > 0) {
        UnloadTexture(textures[0]);
        textures[0] = { 0 };
    }
    if (textures[1].id > 0) {
        UnloadTexture(textures[1]);
        textures[1] = { 0 };
    }
    loaded = false;
    totalFrames = 0;
    currentFrame = 0;
    frameTimer = 0.0f;
    dirPath.clear();
}

void VideoBackground::Update(float dt) {
    if (!loaded || totalFrames <= 0) return;

    int prevFrame = currentFrame;
    frameTimer += dt;
    if (frameTimer >= frameDelay) {
        frameTimer -= frameDelay;
        currentFrame = (currentFrame + 1) % totalFrames;
    }

    if (currentFrame != prevFrame) {
        // Swap buffers: display the one that was being loaded
        int oldDisplay = displayIdx;
        displayIdx = (displayIdx == 0) ? 1 : 0;

        // Start loading the next frame into the now-unused buffer
        int nextFrame = (currentFrame + 1) % totalFrames;
        LoadFrameToTexture(oldDisplay, nextFrame);
    }
}

void VideoBackground::Draw(float fadeAlpha) {
    if (!loaded || textures[displayIdx].id <= 0) return;

    float scaleX = 1080.0f / frameW;
    float scaleY = 720.0f / frameH;
    float scale = (scaleX > scaleY) ? scaleX : scaleY;
    float dw = frameW * scale;
    float dh = frameH * scale;
    float dx = (1080.0f - dw) / 2.0f;
    float dy = (720.0f - dh) / 2.0f;

    unsigned char a = (unsigned char)(fadeAlpha * 255);
    DrawTextureEx(textures[displayIdx], { dx, dy }, 0.0f, scale, { 255, 255, 255, a });
}

void VideoBackground::Restart() {
    currentFrame = 0;
    frameTimer = 0.0f;
    displayIdx = 0;
    if (totalFrames > 0) {
        LoadFrameToTexture(0, 0);
        if (totalFrames > 1) {
            LoadFrameToTexture(1, 1);
        }
    }
}
