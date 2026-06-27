#include "OpeningScene.h"
#include "AssetManager.h"
#include <cmath>

// =============================================
// Mapping audio sesuai urutan dialog:
// voss..11   = 11 dialog Captain Voss
// ace_01..06 = 6 dialog Ace
// nova_01..02 = 2 dialog Nova
// system_1    = RECOVERED AUDIO FRAGMENT
// system_02   = SIGNAL LOST
// Total: 21 dialog
// =============================================

void OpeningScene::InitDialogs() {
    dialogs.clear();

    dialogs.push_back({ Speaker::CAPTAIN_VOSS,
        "Ace, are you reading me? Patch is unstable — keep this short.",
        "voss_01" });
    dialogs.push_back({ Speaker::CAPTAIN_VOSS,
        "We've lost contact with Scout Unit Nova. Last known position: Sector 7, deep in the belt.",
        "voss_02" });
    dialogs.push_back({ Speaker::ACE,
        "Nova? When did she go dark?",
        "ace_01" });
    dialogs.push_back({ Speaker::CAPTAIN_VOSS,
        "Forty-two minutes ago. Right when the first cluster hit.",
        "voss_03" });
    dialogs.push_back({ Speaker::ACE,
        "Any distress beacon? Anything at all?",
        "ace_02" });
    dialogs.push_back({ Speaker::CAPTAIN_VOSS,
        "Static. Nothing but static. That's why I'm sending you in.",
        "voss_04" });
    dialogs.push_back({ Speaker::CAPTAIN_VOSS,
        "The asteroid field is thickening fast. Something is pushing them inward — we don't know what yet.",
        "voss_05" });
    dialogs.push_back({ Speaker::CAPTAIN_VOSS,
        "Your job: hold the line, keep the ship intact, and find any trace of Nova's signal.",
        "voss_06" });
    dialogs.push_back({ Speaker::ACE,
        "Hold the line against how many?",
        "ace_03" });
    dialogs.push_back({ Speaker::CAPTAIN_VOSS,
        "More than yesterday. And they're getting faster.",
        "voss_07" });
    dialogs.push_back({ Speaker::ACE,
        "Great. I love a fair fight.",
        "ace_04" });
    dialogs.push_back({ Speaker::CAPTAIN_VOSS,
        "This isn't a fight, Ace. It's a survival test. Don't lose.",
        "voss_08" });
    dialogs.push_back({ Speaker::SYSTEM,
        "[ RECOVERED AUDIO FRAGMENT — 42 MINUTES AGO ]",
        "system_1" });
    dialogs.push_back({ Speaker::NOVA,
        "Captain... Ace... if you're receiving this... I found something.",
        "nova_01" });
    dialogs.push_back({ Speaker::NOVA,
        "It's not random. The asteroids... they're being directed. Someone is—",
        "nova_02" });
    dialogs.push_back({ Speaker::SYSTEM,
        "[ SIGNAL LOST ]",
        "system_02" });
    dialogs.push_back({ Speaker::CAPTAIN_VOSS,
        "That's all we have. Whatever she found out there... we need to know.",
        "voss_09" });
    dialogs.push_back({ Speaker::ACE,
        "I'm going in. Keep the channel open, Voss.",
        "ace_05" });
    dialogs.push_back({ Speaker::CAPTAIN_VOSS,
        "Eyes open. Fingers ready. And Ace—",
        "voss_10" });
    dialogs.push_back({ Speaker::CAPTAIN_VOSS,
        "Bring her home.",
        "voss_11" });

    DialogLine last;
    last.speaker  = Speaker::ACE;
    last.text     = "Count on it.";
    last.audioKey = "ace_06";
    last.isLast   = true;
    dialogs.push_back(last);
}

OpeningScene::OpeningScene() {
    assetsLoaded      = false;
    audioPlayed       = false;
    currentIndex      = 0;
    isSkipped         = false;
    typewriterTimer   = 0.0f;
    typewriterSpeed   = 38.0f;
    charsVisible      = 0;
    lineComplete      = false;
    dialogBoxAlpha    = 0.0f;
    charAlpha         = 0.0f;
    lightningTimer    = 0.0f;
    bgDimAlpha        = 0.0f;
    fadeOutAlpha      = 0.0f;
    isFadingOut       = false;
    isDialogTransiting  = false;
    dialogTransTimer    = 0.0f;
    transitionTriggered = false;
    openingMusicVol    = 0.5f;
    openingMusicFading = false;
}

OpeningScene::~OpeningScene() {}

Sound openingRadioNoise = {0};
bool openingRadioLoaded = false;

void OpeningScene::Init() {
    InitDialogs();
    auto& am = AssetManager::getInstance();

    // Load radio noise ambient
    if (!openingRadioLoaded && FileExists("assets/sfx/noise_radio.mp3")) {
        openingRadioNoise = LoadSound("assets/sfx/noise_radio.mp3");
        SetSoundVolume(openingRadioNoise, 0.08f);
        openingRadioLoaded = true;
    }

    // Textures
    am.loadTexture("nova",          "assets/img/nova.jpg");
    am.loadTexture("captain_voss",  "assets/img/captain_voss.jpg");
    am.loadTexture("spaceship_ace", "assets/img/spaceship_ace.png");

    // Dialog audio
    am.loadSound("voss_01",   "assets/sound/voss_01.mp3");
    am.loadSound("voss_02",   "assets/sound/voss_02.mp3");
    am.loadSound("voss_03",   "assets/sound/voss_03.mp3");
    am.loadSound("voss_04",   "assets/sound/voss_04.mp3");
    am.loadSound("voss_05",   "assets/sound/voss_05.mp3");
    am.loadSound("voss_06",   "assets/sound/voss_06.mp3");
    am.loadSound("voss_07",   "assets/sound/voss_07.mp3");
    am.loadSound("voss_08",   "assets/sound/voss_08.mp3");
    am.loadSound("voss_09",   "assets/sound/voss_09.mp3");
    am.loadSound("voss_10",   "assets/sound/voss_10.mp3");
    am.loadSound("voss_11",   "assets/sound/voss_11.mp3");
    am.loadSound("ace_01",    "assets/sound/ace_01.mp3");
    am.loadSound("ace_02",    "assets/sound/ace_02.mp3");
    am.loadSound("ace_03",    "assets/sound/ace_03.mp3");
    am.loadSound("ace_04",    "assets/sound/ace_04.mp3");
    am.loadSound("ace_05",    "assets/sound/ace_05.mp3");
    am.loadSound("ace_06",    "assets/sound/ace_06.mp3");
    am.loadSound("nova_01",   "assets/sound/nova_01.mp3");
    am.loadSound("nova_02",   "assets/sound/nova_02.mp3");
    am.loadSound("system_1",  "assets/sound/system_1.mp3");
    am.loadSound("system_02", "assets/sound/system_02.mp3");

    // Video background (pre-extracted PNG frames for reliable playback)
    videoBg.Load("assets/video_frames");

    assetsLoaded = true;

    // Stop opening music — biar nggak bentrok sama suara dialog
    if (am.hasSound("opening_music")) StopSound(am.getSound("opening_music"));
}

void OpeningScene::Unload() {
    if (openingRadioLoaded) {
        StopSound(openingRadioNoise);
        UnloadSound(openingRadioNoise);
        openingRadioLoaded = false;
    }
    videoBg.Unload();
    assetsLoaded = false;
}

// =============================================
// HELPER
// =============================================

const char* OpeningScene::GetSpeakerName(Speaker s) const {
    switch (s) {
        case Speaker::CAPTAIN_VOSS: return "CAPTAIN VOSS";
        case Speaker::ACE:          return "ACE";
        case Speaker::NOVA:         return "NOVA";
        case Speaker::SYSTEM:       return "[ SYSTEM ]";
        default:                    return "";
    }
}

Color OpeningScene::GetSpeakerColor(Speaker s) const {
    switch (s) {
        case Speaker::CAPTAIN_VOSS: return { 255, 165, 60,  255 };
        case Speaker::ACE:          return { 60,  210, 255, 255 };
        case Speaker::NOVA:         return { 200, 110, 255, 255 };
        case Speaker::SYSTEM:       return { 150, 150, 150, 255 };
        default:                    return WHITE;
    }
}

void OpeningScene::PlayDialogAudio() {
    if (audioPlayed) return;
    if (currentIndex >= (int)dialogs.size()) return;
    const std::string& key = dialogs[currentIndex].audioKey;
    if (key.empty()) {
        audioPlayed = true;
        return;
    }
    if (AssetManager::getInstance().hasSound(key)) {
        PlaySound(AssetManager::getInstance().getSound(key));
        audioPlayed = true;
    }
}

void OpeningScene::FadeOutOpeningMusic(float dt) {
    if (!openingMusicFading) return;
    auto& am = AssetManager::getInstance();
    openingMusicVol -= dt * 1.5f;
    if (openingMusicVol <= 0.0f) {
        openingMusicVol = 0.0f;
        openingMusicFading = false;
        if (am.hasSound("opening_music")) StopSound(am.getSound("opening_music"));
    } else {
        if (am.hasSound("opening_music")) {
            SetSoundVolume(am.getSound("opening_music"), openingMusicVol);
        }
    }
}

void OpeningScene::NextDialog() {
    // Stop audio dialog sekarang
    if (currentIndex < (int)dialogs.size()) {
        const std::string& key = dialogs[currentIndex].audioKey;
        if (!key.empty() && AssetManager::getInstance().hasSound(key))
            StopSound(AssetManager::getInstance().getSound(key));
    }

    if (currentIndex >= (int)dialogs.size() - 1) {
        isFadingOut = true;
        openingMusicFading = true;
        return;
    }
    currentIndex++;
    charsVisible    = 0;
    typewriterTimer = 0.0f;
    lineComplete    = false;
    charAlpha       = 0.0f;
    lightningTimer  = 0.0f;
    audioPlayed     = false;
}

void OpeningScene::SkipAll() {
    // Stop semua audio
    for (auto& d : dialogs) {
        if (!d.audioKey.empty() && AssetManager::getInstance().hasSound(d.audioKey))
            StopSound(AssetManager::getInstance().getSound(d.audioKey));
    }
    openingMusicFading = true;
    isSkipped          = true;
    isFadingOut        = true;
}

// =============================================
// UPDATE
// =============================================

void OpeningScene::Update(bool& doneOpening) {
    float dt = GetFrameTime();

    videoBg.Update(dt);
    FadeOutOpeningMusic(dt);

    if (isFadingOut) {
        fadeOutAlpha   += dt * 2.2f;
        dialogBoxAlpha  = fmaxf(0.0f, dialogBoxAlpha - dt * 3.0f);
        charAlpha       = fmaxf(0.0f, charAlpha      - dt * 3.0f);
        if (fadeOutAlpha >= 1.0f) doneOpening = true;
        return;
    }

    if (isDialogTransiting) {
        dialogTransTimer += dt;
        if (dialogTransTimer < 0.2f) {
            float t = dialogTransTimer / 0.2f;
            dialogBoxAlpha = 1.0f - t;
            charAlpha = 1.0f - t;
        } else if (dialogTransTimer < 0.22f && !transitionTriggered) {
            transitionTriggered = true;
            NextDialog();
            dialogBoxAlpha = 0.0f;
            charAlpha = 0.0f;
            if (isFadingOut) return;
        } else {
            float t = (dialogTransTimer - 0.22f) / 0.18f;
            dialogBoxAlpha = fminf(t, 1.0f);
            charAlpha = fminf(t, 1.0f);
            if (dialogTransTimer >= 0.4f) {
                dialogBoxAlpha = 1.0f;
                charAlpha = 1.0f;
                isDialogTransiting = false;
            }
        }
        return;
    }

    bgDimAlpha     = fminf(bgDimAlpha    + dt * 1.5f, 0.82f);
    dialogBoxAlpha = fminf(dialogBoxAlpha + dt * 2.5f, 1.0f);
    charAlpha      = fminf(charAlpha      + dt * 2.0f, 1.0f);
    lightningTimer += dt * 5.0f;

    // Typewriter
    const std::string& fullText = dialogs[currentIndex].text;
    if (!lineComplete) {
        typewriterTimer += dt;
        int target = (int)(typewriterTimer * typewriterSpeed);
        if (target >= (int)fullText.size()) {
            charsVisible = (int)fullText.size();
            lineComplete = true;
        } else {
            charsVisible = target;
        }
    }

    // Play radio noise ambient (low volume, reduced during voice)
    if (openingRadioLoaded && !IsSoundPlaying(openingRadioNoise)) {
        SetSoundVolume(openingRadioNoise, 0.08f);
        PlaySound(openingRadioNoise);
    }

    // Glitch effect at SIGNAL LOST (index 15 = system_02)
    if (dialogs[currentIndex].audioKey == "system_02") {
        if (openingRadioLoaded) {
            SetSoundVolume(openingRadioNoise, 0.25f); // louder static
        }
    } else {
        if (openingRadioLoaded && IsSoundPlaying(openingRadioNoise)) {
            SetSoundVolume(openingRadioNoise, 0.08f);
        }
    }

    // Play audio sekali per dialog
    PlayDialogAudio();

    // Input
    if (IsKeyPressed(KEY_ESCAPE)) { SkipAll(); return; }

    if (IsKeyPressed(KEY_ENTER)) {
        if (!lineComplete) {
            charsVisible = (int)fullText.size();
            lineComplete = true;
        } else if (!isDialogTransiting) {
            isDialogTransiting = true;
            dialogTransTimer = 0.0f;
            transitionTriggered = false;
        }
    }
}

// =============================================
// DRAW
// =============================================

void OpeningScene::DrawLightning(float x, float y, Color c) {
    float a = 0.5f + 0.5f * sinf(lightningTimer);
    Color lc = { c.r, c.g, c.b, (unsigned char)(a * 220) };
    DrawLineEx({ x,     y      }, { x + 5,  y + 9  }, 2.0f, lc);
    DrawLineEx({ x + 5, y + 9  }, { x + 2,  y + 9  }, 2.0f, lc);
    DrawLineEx({ x + 2, y + 9  }, { x + 7,  y + 18 }, 2.0f, lc);
}

void OpeningScene::DrawPortrait() {
    if (!assetsLoaded) return;
    Speaker spk = dialogs[currentIndex].speaker;
    if (spk == Speaker::SYSTEM || spk == Speaker::NONE) return;

    unsigned char alpha = (unsigned char)(charAlpha * 255);
    auto& am = AssetManager::getInstance();

    std::string key;
    if      (spk == Speaker::CAPTAIN_VOSS) key = "captain_voss";
    else if (spk == Speaker::NOVA)         key = "nova";
    else if (spk == Speaker::ACE)          key = "spaceship_ace";

    if (key.empty() || !am.hasTexture(key)) return;
    Texture2D tex = am.getTexture(key);

    float boxY = 720.0f - 200.0f;  // top of dialog box

    float ph, pw, px, py;

    if (spk == Speaker::ACE) {
        // Spaceship — di atas garis dialog, horizontal
        ph = 130.0f;
        pw = ph * ((float)tex.width / tex.height);
        px = 30.0f;
        py = boxY - ph - 12.0f;  // tepat di ATAS garis dialog
    } else {
        // Manusia — portrait vertikal, muncul di atas garis
        ph = 210.0f;
        pw = ph * ((float)tex.width / tex.height);
        px = 30.0f;
        py = boxY - ph - 8.0f;   // tepat di ATAS garis dialog
    }

    // Shadow background di belakang portrait
    DrawRectangle((int)px - 4, (int)py - 4, (int)pw + 8, (int)ph + 8,
        { 0, 0, 0, (unsigned char)(alpha * 0.75f) });

    // Border glow sesuai warna speaker
    Color sc = GetSpeakerColor(spk);
    DrawRectangleLinesEx({ px - 2, py - 2, pw + 4, ph + 4 }, 2,
        { sc.r, sc.g, sc.b, (unsigned char)(alpha * 0.8f) });

    // Gambar portrait
    DrawTextureEx(tex, { px, py }, 0.0f, pw / tex.width,
        { 255, 255, 255, alpha });
}

void OpeningScene::DrawDialogBox() {
    if (currentIndex >= (int)dialogs.size()) return;
    const DialogLine& line = dialogs[currentIndex];
    Speaker spk = line.speaker;

    unsigned char boxAlpha = (unsigned char)(dialogBoxAlpha * 205);
    unsigned char txtAlpha = (unsigned char)(dialogBoxAlpha * 255);

    float boxX = 0.0f;
    float boxY = 720.0f - 200.0f;
    float boxW = 1080.0f;
    float boxH = 200.0f;

    // Box background
    DrawRectangle((int)boxX, (int)boxY, (int)boxW, (int)boxH,
        { 4, 8, 22, boxAlpha });

    // Garis atas warna speaker
    Color sc = GetSpeakerColor(spk);
    DrawRectangle((int)boxX, (int)boxY, (int)boxW, 3,
        { sc.r, sc.g, sc.b, txtAlpha });

    // Nama speaker — mulai dari kanan area portrait (~310px)
    const char* nameStr = GetSpeakerName(spk);
    int nameX = 315;
    int nameY = (int)boxY + 18;

    // Lightning di kiri nama
    if (spk != Speaker::SYSTEM)
        DrawLightning((float)nameX - 26, (float)nameY + 2, sc);

    DrawText(nameStr, nameX, nameY, 22,
        { sc.r, sc.g, sc.b, txtAlpha });

    // Garis bawah nama
    int nw = MeasureText(nameStr, 22);
    DrawRectangle(nameX, nameY + 28, nw + 30, 1,
        { sc.r, sc.g, sc.b, (unsigned char)(txtAlpha * 0.35f) });

    // Teks dialog typewriter dengan word wrap
    std::string visible = line.text.substr(0, charsVisible);
    int maxW   = 715;
    int fsize  = 23;
    int textX  = 315;
    int textY  = nameY + 44;

    std::string word, curLine;
    int lineY = textY;

    for (size_t i = 0; i <= visible.size(); i++) {
        char ch = (i < visible.size()) ? visible[i] : ' ';
        if (ch == ' ' || i == visible.size()) {
            std::string test = curLine.empty() ? word : curLine + " " + word;
            if (MeasureText(test.c_str(), fsize) > maxW) {
                if (!curLine.empty()) {
                    DrawText(curLine.c_str(), textX, lineY, fsize,
                        { 225, 238, 255, txtAlpha });
                    lineY += fsize + 7;
                }
                curLine = word;
            } else {
                curLine = test;
            }
            word.clear();
        } else {
            word += ch;
        }
    }
    if (!curLine.empty())
        DrawText(curLine.c_str(), textX, lineY, fsize,
            { 225, 238, 255, txtAlpha });

    // Hint ENTER berkedip
    if (lineComplete) {
        float pulse = 0.5f + 0.5f * sinf(GetTime() * 4.0f);
        unsigned char ha = (unsigned char)(pulse * txtAlpha);
        const char* hint = line.isLast ? "[ ENTER ]  Start Mission" : "[ ENTER ]  Continue";
        int hw = MeasureText(hint, 15);
        DrawText(hint, 1080 - hw - 28, (int)(boxY + boxH - 26), 15,
            { 140, 175, 210, ha });
        DrawTriangle(
            { (float)(1080 - hw - 46), (float)(boxY + boxH - 19) },
            { (float)(1080 - hw - 46), (float)(boxY + boxH - 27) },
            { (float)(1080 - hw - 38), (float)(boxY + boxH - 23) },
            { 140, 175, 210, ha });
    }

    DrawText("[ ESC ] Skip All", 1080 - 132, 10, 13, { 70, 90, 110, 150 });
    const char* prog = TextFormat("%d / %d", currentIndex + 1, (int)dialogs.size());
    DrawText(prog, 1080 - MeasureText(prog, 12) - 10, (int)boxY + 6, 12,
        { 65, 85, 110, 170 });
}

void OpeningScene::Draw() {
    if (videoBg.IsLoaded()) {
        videoBg.Draw(1.0f - fminf(fadeOutAlpha, 1.0f));
        DrawRectangle(0, 0, 1080, 720,
            { 0, 0, 0, (unsigned char)(bgDimAlpha * 205) });
    } else {
        DrawRectangle(0, 0, 1080, 720,
            { 0, 5, 18, (unsigned char)(bgDimAlpha * 255) });
    }

    DrawPortrait();
    DrawDialogBox();

    if (isFadingOut && fadeOutAlpha > 0.01f)
        DrawRectangle(0, 0, 1080, 720,
            { 0, 0, 0, (unsigned char)(fminf(fadeOutAlpha, 1.0f) * 255) });
}
