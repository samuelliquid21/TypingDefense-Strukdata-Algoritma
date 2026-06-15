#include "LoadingScreen.h"
#include "AssetManager.h"
#include <cmath>

Sound loadingNoise = {0};
bool loadingNoiseLoaded = false;

LoadingScreen::LoadingScreen() {
    phase          = PHASE_FADE_IN;
    timer          = 0.0f;
    fadeAlpha      = 1.0f;
    loadingProgress = 0.0f;
    barPulse       = 0.0f;
    audioTimer     = 0.0f;

    if (!loadingNoiseLoaded && FileExists("assets/sfx/grey_noise.mp3")) {
        loadingNoise = LoadSound("assets/sfx/grey_noise.mp3");
        SetSoundVolume(loadingNoise, 0.15f);
        loadingNoiseLoaded = true;
    }

    quotes = {
        { "Year 2157.",                                       0.0f,   3.5f, 0.0f },
        { "The asteroid belt between Mars and Jupiter",       5.0f,   8.0f, 0.0f },
        { "has become a graveyard of shattered worlds.",       8.0f,  11.5f, 0.0f },
        { "A signal was intercepted. Unknown origin.",        13.0f, 17.5f, 0.0f },
        { "Then she vanished.",                                18.0f, 19.9f, 0.0f },
        { "Your mission: survive the storm.",                 21.0f, 24.0f, 0.0f },
        { "Find Nova. Uncover the truth.",                    24.0f, 27.2f, 0.0f },
        { "Cosmic Keypad",                                    27.0f, 31.0f, 0.0f },
    };

    currentSlide = 0;
    nextSlide = -1;
    transitioning = false;
    transitionTimer = 0.0f;
    slidesLoaded = false;
    bgJpgLoaded = false;
    texBgJpg = {0};
}

LoadingScreen::~LoadingScreen() {
    if (loadingNoiseLoaded) {
        StopSound(loadingNoise);
        UnloadSound(loadingNoise);
        loadingNoiseLoaded = false;
    }
    UnloadStorySlides();
}

void LoadingScreen::LoadStorySlides() {
    struct { const char* folder; const char* file; float show; float end; } defs[] = {
        { "img01", "Pesawat Luar Angkasa Dekat Jupiter.png",  0.0f,  4.5f },
        { "img05", "Kapal Luna Lepas Landas.png",             4.5f,  9.0f },
        { "img07", "Gambar Blackhole.jpg",                    9.0f, 12.5f },
        { "img09", "Cockpit Lost Sinyal.png",                 12.5f, 17.5f },
        { "img16", "Menemukan Sinyal Pesawat Luna.png",       17.5f, 20.5f },
        { "img12", "Target Mission.png",                      20.5f, 24.0f },
        { "img18", "Komunikasi dengan Luna.png",              24.0f, 27.0f },
        { "img14", "Type Defence FIXED.png",                  27.0f, 31.0f },
    };

    for (auto& d : defs) {
        char path[256];
        snprintf(path, sizeof(path), "assets/story/%s/%s", d.folder, d.file);
        if (FileExists(path)) {
            StorySlide slide;
            slide.texture = LoadTexture(path);
            slide.showTime = d.show;
            slide.endTime = d.end;
            storySlides.push_back(slide);
        }
    }

    if (!bgJpgLoaded && FileExists("assets/img/background.jpg")) {
        texBgJpg = LoadTexture("assets/img/background.jpg");
        bgJpgLoaded = true;
    }
}

void LoadingScreen::UnloadStorySlides() {
    for (auto& slide : storySlides) {
        if (slide.texture.id > 0)
            UnloadTexture(slide.texture);
    }
    storySlides.clear();
    if (bgJpgLoaded && texBgJpg.id > 0) {
        UnloadTexture(texBgJpg);
        texBgJpg = {0};
        bgJpgLoaded = false;
    }
}

void LoadingScreen::UpdateSlideshow(float dt) {
    if (storySlides.empty()) return;

    if (transitioning) {
        transitionTimer += dt;
        if (transitionTimer >= TRANSITION_DURATION) {
            transitioning = false;
            currentSlide = nextSlide;
            nextSlide = -1;
        }
        return;
    }

    int targetSlide = currentSlide;
    float t = audioTimer;
    for (size_t i = 0; i < storySlides.size(); i++) {
        if (t >= storySlides[i].showTime && t < storySlides[i].endTime) {
            targetSlide = (int)i;
            break;
        }
    }
    if (t >= storySlides.back().endTime)
        targetSlide = (int)storySlides.size() - 1;

    if (targetSlide != currentSlide) {
        nextSlide = targetSlide;
        transitioning = true;
        transitionTimer = 0.0f;
    }
}

void LoadingScreen::DrawBackground() {
    if (!slidesLoaded) {
        LoadStorySlides();
        slidesLoaded = true;
    }

    // Pas LOADING_BAR dan FADE_OUT pake background.jpg
    if (phase == PHASE_LOADING_BAR || phase == PHASE_FADE_OUT) {
        if (bgJpgLoaded && texBgJpg.id > 0) {
            float scaleX = 1080.0f / texBgJpg.width;
            float scaleY = 720.0f  / texBgJpg.height;
            float scale  = (scaleX > scaleY) ? scaleX : scaleY;
            float dw = texBgJpg.width  * scale;
            float dh = texBgJpg.height * scale;
            float dx = (1080.0f - dw) / 2.0f;
            float dy = (720.0f  - dh) / 2.0f;
            DrawTextureEx(texBgJpg, { dx, dy }, 0.0f, scale, WHITE);
        } else {
            DrawRectangle(0, 0, 1080, 720, { 10, 15, 30, 255 });
        }
        DrawRectangle(0, 0, 1080, 720, { 0, 0, 0, 160 });
        return;
    }

    if (storySlides.empty()) {
        auto& am = AssetManager::Get();
        if (!am.HasTex("loading_bg")) {
            am.LoadTex("loading_bg", "assets/img/loading_bg.png");
        }
        Texture2D& bg = am.GetTex("loading_bg");
        if (bg.id > 0 && bg.width > 0 && bg.height > 0) {
            float scaleX = 1080.0f / bg.width;
            float scaleY = 720.0f  / bg.height;
            float scale  = (scaleX > scaleY) ? scaleX : scaleY;
            float dw = bg.width  * scale;
            float dh = bg.height * scale;
            float dx = (1080.0f - dw) / 2.0f;
            float dy = (720.0f  - dh) / 2.0f;
            DrawTextureEx(bg, { dx, dy }, 0.0f, scale, WHITE);
        } else {
            DrawRectangle(0, 0, 1080, 720, { 10, 15, 30, 255 });
        }
        DrawRectangle(0, 0, 1080, 720, { 0, 0, 0, 160 });
        return;
    }

    auto drawSlide = [](const StorySlide& s, float alpha) {
        if (s.texture.id <= 0) return;
        float scaleX = 1080.0f / s.texture.width;
        float scaleY = 720.0f  / s.texture.height;
        float scale  = (scaleX > scaleY) ? scaleX : scaleY;
        float dw = s.texture.width  * scale;
        float dh = s.texture.height * scale;
        float dx = (1080.0f - dw) / 2.0f;
        float dy = (720.0f  - dh) / 2.0f;
        unsigned char a = (unsigned char)(alpha * 255);
        DrawTextureEx(s.texture, { dx, dy }, 0.0f, scale, { 255, 255, 255, a });
    };

    if (transitioning && nextSlide >= 0 && nextSlide < (int)storySlides.size()) {
        float t = transitionTimer / TRANSITION_DURATION;
        drawSlide(storySlides[nextSlide], t);
        drawSlide(storySlides[currentSlide], 1.0f - t);
    } else {
        drawSlide(storySlides[currentSlide], 1.0f);
    }

    DrawRectangle(0, 0, 1080, 720, { 0, 0, 0, 160 });
}

void LoadingScreen::Update(bool& doneLoading) {
    float dt = GetFrameTime();
    timer    += dt;
    barPulse += dt * 3.0f;

    auto& am = AssetManager::Get();
    if (!am.HasSnd("opening_music")) {
        am.LoadSnd("opening_music", "assets/sound/opening.mp3");
        am.PlaySnd("opening_music");
    }

    if (loadingNoiseLoaded && !IsSoundPlaying(loadingNoise)) {
        PlaySound(loadingNoise);
    }

    switch (phase) {
    case PHASE_FADE_IN:
        fadeAlpha -= dt * 0.7f;
        if (fadeAlpha <= 0.0f) {
            fadeAlpha = 0.0f;
            phase = PHASE_SHOW_QUOTE;
            timer = 0.0f;
        }
        break;

    case PHASE_SHOW_QUOTE: {
        audioTimer += dt;
        UpdateSlideshow(dt);
        for (auto& q : quotes) {
            if (audioTimer < q.startTime) {
                q.alpha = 0.0f;
            } else if (audioTimer < q.startTime + 0.4f) {
                q.alpha = (audioTimer - q.startTime) / 0.4f;
            } else if (audioTimer < q.endTime - 0.4f) {
                q.alpha = 1.0f;
            } else if (audioTimer < q.endTime) {
                q.alpha = (q.endTime - audioTimer) / 0.4f;
            } else {
                q.alpha = 0.0f;
            }
        }
        if (audioTimer >= 30.0f) {
            phase = PHASE_LOADING_BAR;
            timer = 0.0f;
        }
        break;
    }

    case PHASE_LOADING_BAR:
        loadingProgress += dt * 0.38f;
        if (loadingProgress >= 1.0f) {
            loadingProgress = 1.0f;
            phase = PHASE_FADE_OUT;
            timer = 0.0f;
        }
        break;

    case PHASE_FADE_OUT:
        fadeAlpha += dt * 1.5f;
        if (fadeAlpha >= 1.0f) {
            fadeAlpha   = 1.0f;
            if (loadingNoiseLoaded) StopSound(loadingNoise);
            doneLoading = true;
        }
        break;
    }

    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
        if (loadingNoiseLoaded) StopSound(loadingNoise);
        doneLoading = true;
    }
}

void LoadingScreen::DrawQuotes() {
    if (phase != PHASE_SHOW_QUOTE) return;

    for (auto& q : quotes) {
        if (q.alpha <= 0.01f) continue;

        float a = q.alpha * (1.0f - fadeAlpha);
        unsigned char ca = (unsigned char)(a * 255);
        if (ca < 5) continue;

        int fontSize = (q.text == "Cosmic Keypad") ? 48 : 22;
        Color c = (q.text == "Cosmic Keypad")
            ? (Color){ 0, 200, 255, ca }
            : (Color){ 220, 235, 255, ca };

        int tw = MeasureText(q.text.c_str(), fontSize);
        int ty = (q.text == "Cosmic Keypad") ? 300 : 360;
        DrawText(q.text.c_str(), 1080/2 - tw/2, ty, fontSize, c);
    }
}

void LoadingScreen::DrawTitle() {
    if (phase != PHASE_LOADING_BAR && phase != PHASE_FADE_OUT) return;
    float a = (phase == PHASE_LOADING_BAR)
              ? (float)fmin(timer * 2.0f, 1.0f)
              : 1.0f;
    a *= (1.0f - fadeAlpha);
    DrawRectangle(1080/2 - 180, 390, 360, 1,
        { 0, 200, 255, (unsigned char)(a * 60) });
}

void LoadingScreen::DrawLoadingBar() {
    if (phase != PHASE_LOADING_BAR && phase != PHASE_FADE_OUT) return;
    float a = 1.0f - fadeAlpha;

    int barX = 1080/2 - 260;
    int barY = 630;
    int barW = 520;
    int barH = 6;

    DrawRectangle(barX, barY, barW, barH,
        { 30, 50, 70, (unsigned char)(a * 180) });

    float fillW = barW * loadingProgress;
    float pulse = 0.8f + 0.2f * sinf(barPulse);
    DrawRectangle(barX, barY, (int)fillW, barH,
        { 0, (unsigned char)(200 * pulse), (unsigned char)(255 * pulse),
          (unsigned char)(a * 255) });

    if (fillW > 4)
        DrawRectangle(barX + (int)fillW - 3, barY - 2, 6, barH + 4,
            { 180, 240, 255, (unsigned char)(a * 200) });

    const char* pct = TextFormat("%.0f%%", loadingProgress * 100.0f);
    int pw = MeasureText(pct, 13);
    DrawText(pct, 1080/2 - pw/2, barY + 12, 13,
        { 120, 180, 220, (unsigned char)(a * 180) });

    const char* lbl = "LOADING...";
    int lw = MeasureText(lbl, 13);
    DrawText(lbl, 1080/2 - lw/2, barY - 22, 13,
        { 80, 140, 180, (unsigned char)(a * 160) });

    float dotAngle = barPulse * 60.0f * DEG2RAD;
    for (int i = 0; i < 8; i++) {
        float angle = dotAngle + i * (360.0f / 8.0f) * DEG2RAD;
        float dx2 = 1080/2 + cosf(angle) * 28.0f;
        float dy2 = barY - 42 + sinf(angle) * 8.0f;
        float da  = (i == 0) ? 1.0f : (1.0f - i / 8.0f) * 0.5f;
        DrawCircleV({ dx2, dy2 }, 2.5f,
            { 0, 200, 255, (unsigned char)(a * da * 200) });
    }

    DrawText("[ ESC ] Skip", 10, 700, 13, { 60, 80, 100, 140 });
}

void LoadingScreen::Draw() {
    ClearBackground(BLACK);
    DrawBackground();
    DrawQuotes();
    DrawTitle();
    DrawLoadingBar();

    if (fadeAlpha > 0.01f)
        DrawRectangle(0, 0, 1080, 720,
            { 0, 0, 0, (unsigned char)(fadeAlpha * 255) });
}
