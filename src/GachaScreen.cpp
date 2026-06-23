#include "GachaScreen.h"
#include "SkinManager.h"
#include "AssetManager.h"
#include "AudioManager.h"
#include "raylib.h"
#include <cmath>
#include <cstdio>

GachaScreen::GachaScreen() {
    phase = IDLE;
    contentLoaded = false;
    texLoaded = false;
    sndLoaded = false;
    spinSoundPlaying = false;
    scrollOffset = 0.0f;
    spinTimer = 0.0f;
    spinDuration = 0.0f;
    resultTimer = 0.0f;
    rewardId = -1;
    rewardRarity = 0;
    freeSpinUsed = false;
    rewardAlreadyOwned = false;
    rewardGranted = false;
    texZonk1 = {0}; texZonk2 = {0}; texZonk3 = {0}; texZonk4 = {0}; texCoin = {0}; texFreeSpin = {0};
    BuildPool();
}

GachaScreen::~GachaScreen() {
    UnloadContent();
}

void GachaScreen::BuildPool() {
    pool.clear();

    auto addItem = [&](GachaItemType type, int id, int rarity, const char* name, Color color) {
        pool.push_back({ type, id, rarity, name, color });
    };

    // 4 ZONK — id 0..3 (ZONK_1.jpg .. ZONK_4.jpg)
    addItem(GachaItemType::ZONK,     0,  0, "ZONK",            {100,100,100,255});
    addItem(GachaItemType::COIN,     10, 1, "+10 RP",           GOLD);
    addItem(GachaItemType::ZONK,     1,  0, "ZONK",            {110,110,110,255});
    addItem(GachaItemType::COIN,     25, 2, "+25 RP",           GOLD);
    addItem(GachaItemType::ZONK,     2,  0, "ZONK",            {100,100,100,255});
    addItem(GachaItemType::ZONK,     3,  0, "ZONK",            {110,110,110,255});
    addItem(GachaItemType::FREE_SPIN, 0, 3, "FREE SPIN",       {0,255,255,255});
    addItem(GachaItemType::COIN,     50, 2, "+50 RP",          {255,215,0,255});
    addItem(GachaItemType::COIN,     100,3, "+100 RP",         {255,200,0,255});

    // 15 skins
    for (int id = 0; id < SKIN_COUNT; id++) {
        auto& info = getSkinInfo(id);
        addItem(GachaItemType::SKIN, id, info.rarity, info.name.c_str(), info.glowColor);
    }
}

void GachaScreen::LoadContent() {
    if (contentLoaded) return;

    if (!texLoaded) {
        if (FileExists("assets/img/ZONK_1.jpg")) texZonk1 = LoadTexture("assets/img/ZONK_1.jpg");
        if (FileExists("assets/img/ZONK_2.jpg")) texZonk2 = LoadTexture("assets/img/ZONK_2.jpg");
        if (FileExists("assets/img/ZONK_3.jpg")) texZonk3 = LoadTexture("assets/img/ZONK_3.jpg");
        if (FileExists("assets/img/ZONK_4.jpg")) texZonk4 = LoadTexture("assets/img/ZONK_4.jpg");
        if (FileExists("assets/img/KOIN.jpg")) texCoin = LoadTexture("assets/img/KOIN.jpg");
        if (FileExists("assets/img/FREE_SPIN.jpg")) texFreeSpin = LoadTexture("assets/img/FREE_SPIN.jpg");
        texLoaded = true;
    }

    if (!sndLoaded) {
        auto& am = AssetManager::getInstance();
        if (FileExists("assets/gacha/SPIN_WHEELS.wav")) am.loadSound("gacha_spin", "assets/gacha/SPIN_WHEELS.wav");
        if (FileExists("assets/gacha/ZONK.mp3")) am.loadSound("gacha_zonk", "assets/gacha/ZONK.mp3");
        if (FileExists("assets/gacha/COIN.wav")) am.loadSound("gacha_coin", "assets/gacha/COIN.wav");
        if (FileExists("assets/gacha/FREE_SPIN.wav")) am.loadSound("gacha_freespin", "assets/gacha/FREE_SPIN.wav");
        if (FileExists("assets/gacha/COMMON_UNCOMMON.wav")) am.loadSound("gacha_common", "assets/gacha/COMMON_UNCOMMON.wav");
        if (FileExists("assets/gacha/RARE.wav")) am.loadSound("gacha_rare", "assets/gacha/RARE.wav");
        if (FileExists("assets/gacha/EPIC.wav")) am.loadSound("gacha_epic", "assets/gacha/EPIC.wav");
        if (FileExists("assets/gacha/LEGENDARY.wav")) am.loadSound("gacha_legendary", "assets/gacha/LEGENDARY.wav");
        sndLoaded = true;
    }

    contentLoaded = true;
}

void GachaScreen::UnloadContent() {
    if (texLoaded) {
        auto unload = [](Texture2D& t) { if (t.id > 0) UnloadTexture(t); };
        unload(texZonk1); unload(texZonk2); unload(texZonk3); unload(texZonk4); unload(texCoin); unload(texFreeSpin);
        texLoaded = false;
    }
    sndLoaded = false;
    contentLoaded = false;
}

Texture2D* GachaScreen::GetBonusTexture(int poolIdx) {
    if (poolIdx < 0 || poolIdx >= (int)pool.size()) return nullptr;
    auto& item = pool[poolIdx];
    switch (item.type) {
        case GachaItemType::ZONK: {
            switch (item.id) {
                case 0: return &texZonk1;
                case 1: return &texZonk2;
                case 2: return &texZonk3;
                default: return &texZonk4;
            }
        }
        case GachaItemType::COIN:
            return &texCoin;
        case GachaItemType::FREE_SPIN:
            return &texFreeSpin;
        default:
            return nullptr;
    }
}

void GachaScreen::SelectReward() {
    auto& mgr = SkinManager::getInstance();

    bool allCollected = true;
    for (int id = 0; id < SKIN_COUNT; id++) {
        if (!mgr.isUnlocked(id)) { allCollected = false; break; }
    }
    if (allCollected) {
        phase = ALL_COLLECTED;
        return;
    }

    int roll = GetRandomValue(1, 1000);

    // ZONK: 50%  (1-500)
    if (roll <= 500) {
        rewardType = GachaItemType::ZONK;
        rewardId = GetRandomValue(0, 3);
        rewardRarity = 0;
        return;
    }
    // COIN: 35%  (501-850)
    if (roll <= 850) {
        rewardType = GachaItemType::COIN;
        // 501-590: 10, 591-700: 25, 701-790: 50, 791-850: 100
        if (roll <= 590) rewardId = 10;
        else if (roll <= 700) rewardId = 25;
        else if (roll <= 790) rewardId = 50;
        else rewardId = 100;
        rewardRarity = (rewardId <= 25) ? 1 : 2;
        return;
    }
    // FREE_SPIN: 5%  (851-900)
    if (roll <= 900) {
        rewardType = GachaItemType::FREE_SPIN;
        rewardId = 0;
        rewardRarity = 3;
        return;
    }

    // Skins: 10%  (901-1000)
    int skinRoll = roll - 900; // 1-100
    int rarity;
    if (skinRoll <= 50) rarity = 1;        // 5% common
    else if (skinRoll <= 80) rarity = 2;   // 3% uncommon
    else if (skinRoll <= 93) rarity = 3;   // 1.3% rare
    else if (skinRoll <= 98) rarity = 4;   // 0.5% epic
    else rarity = 5;                       // 0.2% legendary

    std::vector<int> candidates;
    for (int id = 0; id < SKIN_COUNT; id++) {
        if (getSkinInfo(id).rarity == rarity && !mgr.isUnlocked(id))
            candidates.push_back(id);
    }
    if (candidates.empty()) {
        for (int id = 0; id < SKIN_COUNT; id++) {
            if (!mgr.isUnlocked(id)) candidates.push_back(id);
        }
        if (candidates.empty()) {
            rewardType = GachaItemType::COIN;
            rewardId = 25;
            rewardRarity = 1;
            return;
        }
    }

    rewardType = GachaItemType::SKIN;
    rewardId = candidates[GetRandomValue(0, (int)candidates.size() - 1)];
    rewardRarity = getSkinInfo(rewardId).rarity;
}

void GachaScreen::StartSpin() {
    auto& mgr = SkinManager::getInstance();

    if (!freeSpinUsed) {
        if (!mgr.spendRP(100)) return;
    }

    rewardGranted = false;
    SelectReward();
    if (phase == ALL_COLLECTED) return;

    int targetIdx = 0;
    for (size_t i = 0; i < pool.size(); i++) {
        if (pool[i].type == rewardType) {
            if (rewardType == GachaItemType::SKIN && pool[i].id == rewardId) {
                targetIdx = (int)i; break;
            }
            if (rewardType == GachaItemType::COIN && pool[i].id == rewardId) {
                targetIdx = (int)i; break;
            }
            if (rewardType == GachaItemType::ZONK && pool[i].id == rewardId) {
                targetIdx = (int)i; break;
            }
            if (pool[i].type == rewardType) {
                targetIdx = (int)i; break;
            }
        }
    }

    float currentPos = scrollOffset;
    float cycles = 5.0f + (float)GetRandomValue(3, 7);
    float totalSpin = cycles * POOL_SIZE;
    float targetPos = floorf(currentPos / POOL_SIZE) * POOL_SIZE + totalSpin + targetIdx;
    while (targetPos <= currentPos + POOL_SIZE * 3) targetPos += POOL_SIZE;

    startOffset = currentPos;
    targetOffset = targetPos;
    spinTimer = 0.0f;
    spinDuration = 3.0f + (float)GetRandomValue(0, 5) * 0.2f;
    phase = SPINNING;

    auto& am = AssetManager::getInstance();
    auto& aud = AudioManager::getInstance();
    if (am.hasSound("gacha_spin")) {
        aud.playSfxOnce("gacha_spin");
        spinSoundPlaying = true;
    }
}

void GachaScreen::UpdateSpin(float dt) {
    spinTimer += dt;
    float t = fminf(spinTimer / spinDuration, 1.0f);
    float eased = 1.0f - powf(1.0f - t, 3.0f);
    scrollOffset = startOffset + (targetOffset - startOffset) * eased;

    if (t >= 1.0f) {
        scrollOffset = targetOffset;
        phase = LANDED;
        resultTimer = 2.5f;

        if (spinSoundPlaying) {
            spinSoundPlaying = false;
            Sound s = AssetManager::getInstance().getSound("gacha_spin");
            if (s.stream.buffer != nullptr) StopSound(s);
        }

        PlayRewardSound();
    }
}

void GachaScreen::PlayRewardSound() {
    auto& am = AssetManager::getInstance();
    auto& aud = AudioManager::getInstance();
    switch (rewardType) {
        case GachaItemType::ZONK:
            if (am.hasSound("gacha_zonk")) aud.playSfx("gacha_zonk"); break;
        case GachaItemType::COIN:
            if (am.hasSound("gacha_coin")) aud.playSfx("gacha_coin"); break;
        case GachaItemType::FREE_SPIN:
            if (am.hasSound("gacha_freespin")) aud.playSfx("gacha_freespin"); break;
        case GachaItemType::SKIN:
            switch (rewardRarity) {
                case 1: case 2:
                    if (am.hasSound("gacha_common")) aud.playSfx("gacha_common"); break;
                case 3:
                    if (am.hasSound("gacha_rare")) aud.playSfx("gacha_rare"); break;
                case 4:
                    if (am.hasSound("gacha_epic")) aud.playSfx("gacha_epic"); break;
                case 5:
                    if (am.hasSound("gacha_legendary")) aud.playSfx("gacha_legendary"); break;
            } break;
    }
}

void GachaScreen::DrawSlot(int poolIdx, float x, float cy, float w, float h, float alpha, bool highlight) {
    if (poolIdx < 0 || poolIdx >= (int)pool.size()) return;
    auto& item = pool[poolIdx];
    unsigned char a = (unsigned char)(alpha * 255);

    Color bg = item.color;
    bg.a = (unsigned char)(alpha * 60);
    Color border = item.color;
    border.a = a;

    if (highlight) {
        float pulse = 1.0f + 0.05f * sinf(GetTime() * 6.0f);
        Rectangle r = { x - w * pulse / 2, cy - h * pulse / 2, w * pulse, h * pulse };
        DrawRectangleRounded(r, 0.08f, 6, { bg.r, bg.g, bg.b, (unsigned char)(alpha * 120) });
        DrawRectangleRoundedLines(r, 0.08f, 6, { border.r, border.g, border.b, a });
    } else {
        DrawRectangleRounded({ x - w/2, cy - h/2, w, h }, 0.08f, 6, bg);
        DrawRectangleRoundedLines({ x - w/2, cy - h/2, w, h }, 0.08f, 6, ColorAlpha(border, alpha * 0.3f));
    }

    if (item.type == GachaItemType::SKIN) {
        auto& mgr = SkinManager::getInstance();
        const auto& tex = mgr.getSpritesheet();
        if (tex.id > 0) {
            auto& info = getSkinInfo(item.id);
            float fw = (float)tex.width / GRID_COLS;
            float fh = (float)tex.height / GRID_ROWS;
            Rectangle src = { info.col * fw, info.row * fh, fw, fh };
            float scale = (h * 0.45f) / fh;
            Rectangle dest = { x, cy - 8, fw * scale, fh * scale };
            Vector2 origin = { dest.width/2, dest.height/2 };
            DrawTexturePro(tex, src, dest, origin, 0.0f, ColorAlpha(WHITE, alpha));
        }
    } else {
        Texture2D* tex = GetBonusTexture(poolIdx);
        if (tex && tex->id > 0) {
            float scale = (h * 0.5f) / fmaxf(tex->width, tex->height);
            float dw = tex->width * scale;
            float dh = tex->height * scale;
            DrawTexturePro(*tex,
                { 0, 0, (float)tex->width, (float)tex->height },
                { x, cy - 6, dw, dh },
                { dw/2, dh/2 }, 0.0f, ColorAlpha(WHITE, alpha));
        }
    }

    int stars = (item.type == GachaItemType::SKIN) ? item.rarity : 0;
    if (stars > 0) {
        char starStr[16] = {0};
        for (int s = 0; s < stars && s < 10; s++) starStr[s] = '*';
        int fontSize = (int)(12 * fminf(1.0f, w / 110.0f));
        int tw = MeasureText(starStr, fontSize);
        DrawText(starStr, (int)(x - tw/2), (int)(cy + h/2 - 22), fontSize, ColorAlpha(GOLD, alpha * 0.8f));
    }

    const char* label = (item.type == GachaItemType::SKIN) ? item.name : pool[poolIdx].name;
    int fontSize = (int)(13 * fminf(1.0f, w / 110.0f));
    int tw = MeasureText(label, fontSize);
    DrawText(label, (int)(x - tw/2), (int)(cy + h/2 - 8), fontSize, ColorAlpha(WHITE, alpha));
}

void GachaScreen::DrawCarousel() {
    float centerX = 540.0f;
    float centerY = 300.0f;
    float itemW = 110.0f;
    float itemH = 150.0f;
    float pitch = 128.0f;
    int halfSlots = 5;

    int baseIdx = (int)floorf(scrollOffset);
    float frac = scrollOffset - baseIdx;

    for (int i = -halfSlots; i <= halfSlots; i++) {
        int idx = ((baseIdx + i) % POOL_SIZE + POOL_SIZE) % POOL_SIZE;
        float x = centerX - frac * pitch + i * pitch;

        if (x + itemW < 0 || x - itemW > 1080) continue;

        float dist = fabsf(i - frac);
        float alpha = fmaxf(0.25f, 1.0f - dist * 0.15f);
        float scale = fmaxf(0.7f, 1.0f - dist * 0.08f);

        bool highlight = (phase == LANDED && i == 0 && dist < 0.1f);
        DrawSlot(idx, x, centerY, itemW * scale, itemH * scale, alpha, highlight);
    }
}

void GachaScreen::DrawResultScreen() {
    float cx = 540.0f;
    float cy = 350.0f;

    if (rewardType == GachaItemType::SKIN) {
        auto& mgr = SkinManager::getInstance();
        auto& info = getSkinInfo(rewardId);
        const auto& tex = mgr.getSpritesheet();
        float fw = (float)tex.width / GRID_COLS;
        float fh = (float)tex.height / GRID_ROWS;
        Rectangle src = { info.col * fw, info.row * fh, fw, fh };

        float pulse = 1.0f + 0.03f * sinf(GetTime() * 4.0f);
        float scale = pulse * (220.0f / fh);
        Rectangle dest = { cx, cy - 30, fw * scale, fh * scale };
        Vector2 origin = { dest.width/2, dest.height/2 };
        DrawTexturePro(tex, src, dest, origin, 0.0f, WHITE);

        Color glow = gachaGlow(rewardType, rewardRarity);
        float glowR = 140 + 30 * sinf(GetTime() * 3.0f);
        DrawCircleV({ cx, cy - 30 }, glowR, glow);

        if (rewardAlreadyOwned) {
            DrawText("ALREADY OWNED", cx - MeasureText("ALREADY OWNED", 24)/2, (int)cy + 110, 24, ColorAlpha(WHITE, 0.6f));
        } else {
            DrawText("NEW SKIN UNLOCKED!", cx - MeasureText("NEW SKIN UNLOCKED!", 28)/2, (int)cy + 110, 28, GOLD);
        }
        DrawText(info.name.c_str(), cx - MeasureText(info.name.c_str(), 24)/2, (int)cy + 150, 24, info.glowColor);

        char starStr[16] = {0};
        for (int s = 0; s < info.rarity; s++) starStr[s] = '*';
        DrawText(starStr, cx - MeasureText(starStr, 20)/2, (int)cy + 185, 20, GOLD);

        const char* rLabel = rarityLabel(info.rarity);
        DrawText(rLabel, cx - MeasureText(rLabel, 18)/2, (int)cy + 215, 18, rarityColor(info.rarity));

    } else if (rewardType == GachaItemType::ZONK) {
        Texture2D* tex = &texZonk1;
        switch (rewardId) {
            case 0: tex = &texZonk1; break;
            case 1: tex = &texZonk2; break;
            case 2: tex = &texZonk3; break;
            case 3: tex = &texZonk4; break;
        }
        if (tex && tex->id > 0) {
            float dispH = 220.0f;
            float scale = dispH / tex->height;
            float dispW = tex->width * scale;
            DrawTexturePro(*tex,
                { 0, 0, (float)tex->width, (float)tex->height },
                { cx, cy - 20, dispW, dispH },
                { dispW/2, dispH/2 }, 0.0f, WHITE);
        }
        DrawText("ZONK!", cx - MeasureText("ZONK!", 48)/2, (int)cy + 120, 48, {180,180,180,255});
        DrawText("Better luck next time...", cx - MeasureText("Better luck next time...", 18)/2, (int)cy + 170, 18, ColorAlpha(WHITE, 0.6f));

    } else if (rewardType == GachaItemType::COIN) {
        Texture2D* tex = &texCoin;
        if (tex && tex->id > 0) {
            float dispH = 220.0f;
            float scale = dispH / tex->height;
            float dispW = tex->width * scale;
            DrawTexturePro(*tex,
                { 0, 0, (float)tex->width, (float)tex->height },
                { cx, cy - 20, dispW, dispH },
                { dispW/2, dispH/2 }, 0.0f, WHITE);
        }
        char coinText[32];
        snprintf(coinText, sizeof(coinText), "+%d RP!", rewardId);
        DrawText(coinText, cx - MeasureText(coinText, 44)/2, (int)cy + 120, 44, GOLD);
        DrawText("Research Points acquired", cx - MeasureText("Research Points acquired", 18)/2, (int)cy + 170, 18, ColorAlpha(WHITE, 0.6f));

    } else if (rewardType == GachaItemType::FREE_SPIN) {
        Texture2D* tex = &texFreeSpin;
        if (tex && tex->id > 0) {
            float dispH = 220.0f;
            float scale = dispH / tex->height;
            float dispW = tex->width * scale;
            DrawTexturePro(*tex,
                { 0, 0, (float)tex->width, (float)tex->height },
                { cx, cy - 20, dispW, dispH },
                { dispW/2, dispH/2 }, 0.0f, WHITE);
        }
        DrawText("FREE SPIN!", cx - MeasureText("FREE SPIN!", 40)/2, (int)cy + 120, 40, SKYBLUE);
        DrawText("One free pull awarded!", cx - MeasureText("One free pull awarded!", 18)/2, (int)cy + 170, 18, ColorAlpha(WHITE, 0.6f));
    }

    DrawText("[ENTER] Continue", cx - MeasureText("[ENTER] Continue", 18)/2, (int)cy + 245, 18, YELLOW);
}

void GachaScreen::Update(bool& backToSkinSelect, bool& goToSkinSelect) {
    backToSkinSelect = false;
    goToSkinSelect = false;
    float dt = GetFrameTime();

    if (!contentLoaded) LoadContent();

    switch (phase) {
        case IDLE: {
            // Carousel diam sampai ENTER ditekan — biar keren
            auto& mgr = SkinManager::getInstance();
            bool hasFree = mgr.getFreeSpinCount() > 0;

            if (IsKeyPressed(KEY_ENTER)) {
                Sound s = AssetManager::getInstance().getSound("gacha_spin");
                if (s.stream.buffer != nullptr) StopSound(s);
                if (hasFree) {
                    mgr.useFreeSpin();
                    freeSpinUsed = true;
                } else {
                    freeSpinUsed = false;
                }
                StartSpin();
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                backToSkinSelect = true;
            }
            break;
        }

        case SPINNING:
            UpdateSpin(dt);
            break;

        case LANDED: {
            resultTimer -= dt;

            if (!rewardGranted) {
                if (rewardType == GachaItemType::COIN) {
                    SkinManager::getInstance().addRP(rewardId);
                } else if (rewardType == GachaItemType::FREE_SPIN) {
                    SkinManager::getInstance().addFreeSpin(1);
                } else if (rewardType == GachaItemType::SKIN) {
                    auto& mgr = SkinManager::getInstance();
                    rewardAlreadyOwned = mgr.isUnlocked(rewardId);
                    mgr.unlockSkin(rewardId);
                    mgr.setActiveSkin(rewardId);
                }
                SkinManager::getInstance().save();
                rewardGranted = true;
            }

            if (resultTimer <= 0 || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ESCAPE)) {
                phase = RESULT;
                resultTimer = 3.0f;
            }
            break;
        }

        case RESULT: {
            resultTimer -= dt;
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ESCAPE) || resultTimer <= 0.0f) {
                phase = IDLE;
                freeSpinUsed = false;
                goToSkinSelect = true;
            }
            break;
        }

        case ALL_COLLECTED: {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
                phase = IDLE;
                backToSkinSelect = true;
            }
            break;
        }
    }
}

void GachaScreen::Draw() {
    auto& mgr = SkinManager::getInstance();

    DrawText("RESEARCH CRATE", 1080/2 - MeasureText("RESEARCH CRATE", 36)/2, 30, 36, WHITE);

    char infoText[64];
    snprintf(infoText, sizeof(infoText), "RP: %d", mgr.getRP());
    DrawText(infoText, 1080/2 - MeasureText(infoText, 22)/2, 70, 22, GOLD);

    if (mgr.getFreeSpinCount() > 0) {
        char freeText[32];
        snprintf(freeText, sizeof(freeText), "Free Spins: %d", mgr.getFreeSpinCount());
        DrawText(freeText, 1080/2 - MeasureText(freeText, 16)/2, 96, 16, SKYBLUE);
    }

    if (phase == IDLE) {
        DrawCarousel();

        const char* action = mgr.getFreeSpinCount() > 0
            ? "[ENTER] Use Free Spin"
            : "[ENTER] Open Crate (100 RP)";
        DrawText(action, 1080/2 - MeasureText(action, 20)/2, 560, 20, YELLOW);

        const char* esc = "[ESC] Back";
        DrawText(esc, 1080/2 - MeasureText(esc, 14)/2, 590, 14, ColorAlpha(WHITE, 0.5f));

        int total = (int)mgr.getUnlockedSkins().size();
        DrawText(TextFormat("Skins: %d/15", total), 1080/2 - MeasureText(TextFormat("Skins: %d/15", total), 16)/2, 620, 16, ColorAlpha(WHITE, 0.5f));

    } else if (phase == SPINNING) {
        DrawCarousel();
        DrawText("SPINNING...", 1080/2 - MeasureText("SPINNING...", 20)/2, 560, 20, ColorAlpha(ORANGE, 0.8f));

    } else if (phase == LANDED) {
        DrawCarousel();
        DrawText("RESULT!", 1080/2 - MeasureText("RESULT!", 28)/2, 540, 28, GOLD);

    } else if (phase == RESULT) {
        DrawResultScreen();

    } else if (phase == ALL_COLLECTED) {
        float cx = 540.0f, cy = 360.0f;
        const char* msg = "ALL SKINS COLLECTED!";
        DrawText(msg, cx - MeasureText(msg, 32)/2, cy - 20, 32, GOLD);
        const char* sub = "You have every skin!";
        DrawText(sub, cx - MeasureText(sub, 18)/2, cy + 30, 18, ColorAlpha(WHITE, 0.6f));
        const char* cont = "[ENTER] Back";
        DrawText(cont, cx - MeasureText(cont, 16)/2, cy + 80, 16, YELLOW);
    }
}
