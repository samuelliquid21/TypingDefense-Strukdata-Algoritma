#pragma once
#include "raylib.h"
#include "AssetManager.h"
#include <string>
#include <vector>

class LoadingScreen {
public:
    LoadingScreen();
    ~LoadingScreen();

    void Update(bool& doneLoading);
    void Draw();

private:
    enum Phase {
        PHASE_FADE_IN,
        PHASE_SHOW_QUOTE,
        PHASE_LOADING_BAR,
        PHASE_FADE_OUT
    };

    Phase phase;
    float timer;
    float fadeAlpha;
    float loadingProgress;
    float barPulse;

    struct QuoteLine {
        std::string text;
        float startTime;
        float endTime;
        float alpha;
    };
    std::vector<QuoteLine> quotes;
    float audioTimer;

    // Background jpg untuk loading bar
    Texture2D texBgJpg;
    bool bgJpgLoaded;

    // Story slideshow
    struct StorySlide {
        Texture2D texture;
        float showTime;
        float endTime;
    };
    std::vector<StorySlide> storySlides;
    bool slidesLoaded;
    int currentSlide;
    int nextSlide;
    bool transitioning;
    float transitionTimer;
    static constexpr float TRANSITION_DURATION = 0.8f;

    void DrawBackground();
    void DrawQuotes();
    void DrawLoadingBar();
    void DrawTitle();
    void LoadStorySlides();
    void UnloadStorySlides();
    void UpdateSlideshow(float dt);
};
