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
        { "has become a graveyard of shattered worlds.",      8.0f,  11.5f, 0.0f },
        { "A signal was intercepted. Unknown origin.",        13.0f, 17.5f, 0.0f },
        { "Then she vanished.",                                18.0f, 19.9f, 0.0f },
        { "Your mission: survive the storm.",                 21.0f, 24.0f, 0.0f },
        { "Find Nova. Uncover the truth.",                    24.0f, 27.2f, 0.0f },
        { "Cosmic Keypad",                                    27.0f, 31.0f, 0.0f },
    };
}

LoadingScreen::~LoadingScreen() {
    if (loadingNoiseLoaded) {
        StopSound(loadingNoise);
        UnloadSound(loadingNoise);
        loadingNoiseLoaded = false;
    }
}

// ---- DRAW BACKGROUND ----
void LoadingScreen::DrawBackground() {
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

    // Overlay gelap supaya teks terbaca
    DrawRectangle(0, 0, 1080, 720, { 0, 0, 0, 160 });
}

// ---- UPDATE ----
void LoadingScreen::Update(bool& doneLoading) {
    float dt = GetFrameTime();
    timer    += dt;
    barPulse += dt * 3.0f;

    // Play opening music
    auto& am = AssetManager::Get();
    if (!am.HasSnd("opening_music")) {
        am.LoadSnd("opening_music", "assets/sound/opening.mp3");
        am.PlaySnd("opening_music");
    }

    // Play grey noise ambient
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

    // ESC / ENTER skip — langsung ke OpeningScene
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
        if (loadingNoiseLoaded) StopSound(loadingNoise);
        doneLoading = true;
    }
}

// ---- DRAW QUOTES ----
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

// ---- DRAW TITLE ----
void LoadingScreen::DrawTitle() {
    if (phase != PHASE_LOADING_BAR && phase != PHASE_FADE_OUT) return;
    float a = (phase == PHASE_LOADING_BAR)
              ? (float)fmin(timer * 2.0f, 1.0f)
              : 1.0f;
    a *= (1.0f - fadeAlpha);
    // Tidak tampilkan judul karena sudah ada di background image
    // Hanya tampilkan garis dekoratif tipis
    DrawRectangle(1080/2 - 180, 390, 360, 1,
        { 0, 200, 255, (unsigned char)(a * 60) });
}

// ---- DRAW LOADING BAR ----
void LoadingScreen::DrawLoadingBar() {
    if (phase != PHASE_LOADING_BAR && phase != PHASE_FADE_OUT) return;
    float a = 1.0f - fadeAlpha;

    int barX = 1080/2 - 260;
    int barY = 630;
    int barW = 520;
    int barH = 6;

    // Track bar
    DrawRectangle(barX, barY, barW, barH,
        { 30, 50, 70, (unsigned char)(a * 180) });

    // Fill
    float fillW = barW * loadingProgress;
    float pulse = 0.8f + 0.2f * sinf(barPulse);
    DrawRectangle(barX, barY, (int)fillW, barH,
        { 0, (unsigned char)(200 * pulse), (unsigned char)(255 * pulse),
          (unsigned char)(a * 255) });

    // Glow ujung
    if (fillW > 4)
        DrawRectangle(barX + (int)fillW - 3, barY - 2, 6, barH + 4,
            { 180, 240, 255, (unsigned char)(a * 200) });

    // Persentase
    const char* pct = TextFormat("%.0f%%", loadingProgress * 100.0f);
    int pw = MeasureText(pct, 13);
    DrawText(pct, 1080/2 - pw/2, barY + 12, 13,
        { 120, 180, 220, (unsigned char)(a * 180) });

    // Label
    const char* lbl = "LOADING...";
    int lw = MeasureText(lbl, 13);
    DrawText(lbl, 1080/2 - lw/2, barY - 22, 13,
        { 80, 140, 180, (unsigned char)(a * 160) });

    // Spinning dots
    float dotAngle = barPulse * 60.0f * DEG2RAD;
    for (int i = 0; i < 8; i++) {
        float angle = dotAngle + i * (360.0f / 8.0f) * DEG2RAD;
        float dx2 = 1080/2 + cosf(angle) * 28.0f;
        float dy2 = barY - 42 + sinf(angle) * 8.0f;
        float da  = (i == 0) ? 1.0f : (1.0f - i / 8.0f) * 0.5f;
        DrawCircleV({ dx2, dy2 }, 2.5f,
            { 0, 200, 255, (unsigned char)(a * da * 200) });
    }

    // Hint skip
    DrawText("[ ESC ] Skip", 10, 700, 13, { 60, 80, 100, 140 });
}

// ---- DRAW ----
void LoadingScreen::Draw() {
    ClearBackground(BLACK);
    DrawBackground();
    DrawQuotes();
    DrawTitle();
    DrawLoadingBar();

    // Fade overlay
    if (fadeAlpha > 0.01f)
        DrawRectangle(0, 0, 1080, 720,
            { 0, 0, 0, (unsigned char)(fadeAlpha * 255) });
}
