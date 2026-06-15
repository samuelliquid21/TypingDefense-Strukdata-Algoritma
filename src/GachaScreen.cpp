#include "GachaScreen.h"
#include "SkinManager.h"
#include "SkinData.h"
#include "raylib.h"
#include <cmath>

GachaScreen::GachaScreen() {
    phase = GachaPhase::IDLE;
    phaseTimer = 0.0f;
    resultSkinId = -1;
    shakeOffset = 0.0f;
    flipProgress = 0.0f;
    resultDisplayTimer = 0.0f;
}

GachaScreen::~GachaScreen() {}

void GachaScreen::startPull() {
    auto& mgr = SkinManager::getInstance();

    if (!mgr.spendRP(100)) return;

    int result = mgr.gachaPull();
    if (result == -1) {
        mgr.addRP(100);
        phase = GachaPhase::ALL_COLLECTED;
        return;
    }

    resultSkinId = result;
    phase = GachaPhase::SHAKING;
    phaseTimer = 1.0f;
    shakeOffset = 0.0f;
    flipProgress = 0.0f;
}

void GachaScreen::Update(bool& backToSkinSelect, bool& goToSkinSelect) {
    backToSkinSelect = false;
    goToSkinSelect = false;

    float dt = GetFrameTime();

    if (phase == GachaPhase::IDLE) {
        if (IsKeyPressed(KEY_ENTER)) {
            startPull();
        }
        if (IsKeyPressed(KEY_ESCAPE)) {
            backToSkinSelect = true;
        }
    } else if (phase == GachaPhase::SHAKING) {
        phaseTimer -= dt;
        shakeOffset = sin(GetTime() * 40.0f) * (phaseTimer * 12.0f);
        if (phaseTimer <= 0.0f) {
            phase = GachaPhase::OPENING;
            phaseTimer = 0.5f;
        }
    } else if (phase == GachaPhase::OPENING) {
        phaseTimer -= dt;
        if (phaseTimer <= 0.0f) {
            phase = GachaPhase::FLIPPING;
            phaseTimer = 0.8f;
            flipProgress = 0.0f;
        }
    } else if (phase == GachaPhase::FLIPPING) {
        phaseTimer -= dt;
        flipProgress = 1.0f - (phaseTimer / 0.8f);
        if (phaseTimer <= 0.0f) {
            auto& mgr = SkinManager::getInstance();
            mgr.unlockSkin(resultSkinId);
            if (resultSkinId >= 0) mgr.setActiveSkin(resultSkinId);
            mgr.save();

            phase = GachaPhase::RESULT;
            resultDisplayTimer = 2.0f;
        }
    } else if (phase == GachaPhase::RESULT) {
        resultDisplayTimer -= dt;
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ESCAPE) || resultDisplayTimer <= 0.0f) {
            phase = GachaPhase::IDLE;
            resultSkinId = -1;
            goToSkinSelect = true;
        }
    } else if (phase == GachaPhase::ALL_COLLECTED) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
            phase = GachaPhase::IDLE;
            backToSkinSelect = true;
        }
    }
}

void GachaScreen::Draw() {
    auto& mgr = SkinManager::getInstance();

    DrawText("RESEARCH CRATE", 1080 / 2 - MeasureText("RESEARCH CRATE", 40) / 2, 60, 40, WHITE);

    char rpText[32];
    snprintf(rpText, sizeof(rpText), "RP: %d", mgr.getRP());
    DrawText(rpText, 1080 / 2 - MeasureText(rpText, 24) / 2, 110, 24, GOLD);

    if (phase == GachaPhase::IDLE) {
        float cx = 1080 / 2;
        float cy = 360;
        float bw = 220;
        float bh = 180;

        DrawRectangleRounded({ cx - bw / 2, cy - bh / 2, bw, bh }, 0.1f, 8, { 60, 50, 40, 255 });
        DrawRectangleRoundedLines({ cx - bw / 2, cy - bh / 2, bw, bh }, 0.1f, 8, DARKGRAY);

        const char* crate = "CRATE";
        DrawText(crate, cx - MeasureText(crate, 28) / 2, cy - 20, 28, WHITE);

        DrawText("[100 RP]", cx - MeasureText("[100 RP]", 16) / 2, cy + 20, 16, GOLD);

        const char* hint = "[ENTER] Open Crate";
        DrawText(hint, cx - MeasureText(hint, 18) / 2, cy + bh / 2 + 30, 18, YELLOW);

        const char* esc = "[ESC] Back";
        DrawText(esc, cx - MeasureText(esc, 14) / 2, cy + bh / 2 + 55, 14, ColorAlpha(WHITE, 0.5f));

        int unlockedCount = (int)mgr.getUnlockedSkins().size();
        DrawText(TextFormat("Unlocked: %d/15", unlockedCount), 1080 / 2 - MeasureText(TextFormat("Unlocked: %d/15", unlockedCount), 18) / 2, 560, 18, ColorAlpha(WHITE, 0.6f));

    } else if (phase == GachaPhase::SHAKING) {
        float cx = 1080 / 2 + shakeOffset;
        float cy = 360;
        float bw = 220;
        float bh = 180;

        DrawRectangleRounded({ cx - bw / 2, cy - bh / 2, bw, bh }, 0.1f, 8, { 80, 40, 30, 255 });
        DrawRectangleRoundedLines({ cx - bw / 2, cy - bh / 2, bw, bh }, 0.1f, 8, ORANGE);

        const char* txt = "? ? ?";
        DrawText(txt, cx - MeasureText(txt, 32) / 2, cy - 10, 32, ORANGE);
        DrawText("Opening...", cx - MeasureText("Opening...", 16) / 2, cy + 40, 16, ColorAlpha(WHITE, 0.7f));

    } else if (phase == GachaPhase::OPENING) {
        float cx = 1080 / 2;
        float cy = 360;
        float intensity = phaseTimer / 0.5f;

        for (int i = 0; i < 20; i++) {
            float angle = (float)i / 20 * 360 * DEG2RAD + GetTime() * 3.0f;
            float dist = 100 + (1.0f - intensity) * 80;
            float px = cx + cos(angle) * dist;
            float py = cy + sin(angle) * dist;
            DrawCircleV({ px, py }, 3 + intensity * 4, ORANGE);
        }

        DrawText("?", cx - 20, cy - 20, 40, WHITE);

    } else if (phase == GachaPhase::FLIPPING) {
        float cx = 1080 / 2;
        float cy = 360;

        float scaleX = fabs(cos(flipProgress * PI / 2));
        float cardW = 200 * scaleX;
        float cardH = 240;

        if (scaleX > 0.05f) {
            Rectangle cardRect = { cx - cardW / 2, cy - cardH / 2, cardW, cardH };
            Color cardColor = (flipProgress < 0.5f) ? DARKGRAY : GRAY;
            DrawRectangleRounded(cardRect, 0.08f, 8, cardColor);
            DrawRectangleRoundedLines(cardRect, 0.08f, 8, WHITE);

            if (flipProgress >= 0.5f) {
                const SkinInfo& info = getSkinInfo(resultSkinId);

                DrawRectangleRounded(cardRect, 0.08f, 8, ColorAlpha(WHITE, 0.05f));

                Rectangle shipSrc;
                const auto& tex = mgr.getSpritesheet();
                float fw = (float)tex.width / GRID_COLS;
                float fh = (float)tex.height / GRID_ROWS;
                shipSrc = { (float)info.col * fw, (float)info.row * fh, fw, fh };

                float shipScale = (cardW * 0.7f) / fw;
                Rectangle dest = { cx, cy - 20, fw * shipScale, fh * shipScale };
                Vector2 origin = { dest.width / 2, dest.height / 2 };
                DrawTexturePro(tex, shipSrc, dest, origin, 0.0f, WHITE);

                char nameText[64];
                snprintf(nameText, sizeof(nameText), "%s", info.name.c_str());
                DrawText(nameText, cx - MeasureText(nameText, 16) / 2, cy + cardH / 2 - 40, 16, info.glowColor);

                char starText[16];
                int starCount = info.rarity;
                int si = 0;
                for (int s = 0; s < starCount; s++) {
                    starText[si++] = '*';
                }
                starText[si] = '\0';
                DrawText(starText, cx - MeasureText(starText, 14) / 2, cy + cardH / 2 - 22, 14, GOLD);
            }
        }

    } else if (phase == GachaPhase::RESULT) {
        float cx = 1080 / 2;
        float cy = 300;

        const SkinInfo& info = getSkinInfo(resultSkinId);
        const auto& tex = mgr.getSpritesheet();
        float fw = (float)tex.width / GRID_COLS;
        float fh = (float)tex.height / GRID_ROWS;
        Rectangle shipSrc = { (float)info.col * fw, (float)info.row * fh, fw, fh };

        float pulse = 1.0f + 0.03f * sin(GetTime() * 4.0f);
        float shipScale = pulse * (200.0f / fw);
        Rectangle dest = { cx, cy, fw * shipScale, fh * shipScale };
        Vector2 origin = { dest.width / 2, dest.height / 2 };
        DrawTexturePro(tex, shipSrc, dest, origin, 0.0f, WHITE);

        Color glow = rarityGlow(info.rarity);
        float glowR = 100 + 30 * sin(GetTime() * 3.0f);
        DrawCircleV({ cx, cy }, glowR, glow);

        const char* newText = "NEW SKIN UNLOCKED!";
        DrawText(newText, cx - MeasureText(newText, 28) / 2, cy + 120, 28, GOLD);

        char nameText[64];
        snprintf(nameText, sizeof(nameText), "%s", info.name.c_str());
        DrawText(nameText, cx - MeasureText(nameText, 24) / 2, cy + 160, 24, info.glowColor);

        char starText[16];
        int si = 0;
        for (int s = 0; s < info.rarity; s++) starText[si++] = '*';
        starText[si] = '\0';
        DrawText(starText, cx - MeasureText(starText, 20) / 2, cy + 195, 20, GOLD);

        const char* rarityTxt = rarityLabel(info.rarity);
        DrawText(rarityTxt, cx - MeasureText(rarityTxt, 18) / 2, cy + 225, 18, rarityColor(info.rarity));

        if (resultDisplayTimer > 0) {
            char timerText[32];
            snprintf(timerText, sizeof(timerText), "(%.0f)", resultDisplayTimer);
            DrawText(timerText, cx - MeasureText(timerText, 14) / 2, 560, 14, ColorAlpha(WHITE, 0.4f));
        }

        const char* cont = "[ENTER] Continue";
        DrawText(cont, cx - MeasureText(cont, 16) / 2, 590, 16, YELLOW);

    } else if (phase == GachaPhase::ALL_COLLECTED) {
        float cx = 1080 / 2;
        float cy = 360;

        const char* msg = "ALL SKINS COLLECTED!";
        DrawText(msg, cx - MeasureText(msg, 32) / 2, cy - 20, 32, GOLD);

        const char* sub = "You have every skin!";
        DrawText(sub, cx - MeasureText(sub, 18) / 2, cy + 30, 18, ColorAlpha(WHITE, 0.6f));

        const char* cont = "[ENTER] Back";
        DrawText(cont, cx - MeasureText(cont, 16) / 2, cy + 80, 16, YELLOW);
    }
}
