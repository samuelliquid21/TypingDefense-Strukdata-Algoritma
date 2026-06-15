#pragma once
#include "raylib.h"
#include "AssetManager.h"
#include "VideoBackground.h"
#include <string>
#include <vector>

enum class Speaker {
    NONE,
    CAPTAIN_VOSS,
    ACE,
    NOVA,
    SYSTEM
};

struct DialogLine {
    Speaker     speaker;
    std::string text;
    std::string audioKey;
    bool        isLast = false;
};

class OpeningScene {
public:
    OpeningScene();
    ~OpeningScene();

    void Init();
    void Unload();
    void Update(bool& doneOpening);
    void Draw();

private:
    bool assetsLoaded;
    bool audioPlayed;

    std::vector<DialogLine> dialogs;
    int   currentIndex;
    bool  isSkipped;

    // Typewriter
    float typewriterTimer;
    float typewriterSpeed;
    int   charsVisible;
    bool  lineComplete;

    // UI animation
    float dialogBoxAlpha;
    float charAlpha;
    float lightningTimer;
    float bgDimAlpha;

    // Fade
    float fadeOutAlpha;
    bool  isFadingOut;

    // Dialog transition fade
    bool  isDialogTransiting;
    float dialogTransTimer;

    // Opening music fade
    float openingMusicVol;
    bool  openingMusicFading;

    // Video background
    VideoBackground videoBg;

    const char* GetSpeakerName(Speaker s) const;
    Color       GetSpeakerColor(Speaker s) const;
    void        DrawDialogBox();
    void        DrawPortrait();
    void        DrawLightning(float x, float y, Color c);
    void        NextDialog();
    void        SkipAll();
    void        InitDialogs();
    void        PlayDialogAudio();
    void        FadeOutOpeningMusic(float dt);
};
