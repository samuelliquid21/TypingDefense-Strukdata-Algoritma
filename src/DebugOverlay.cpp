#include "DebugOverlay.h"
#include "raylib.h"
#include "GameConfig.h"

// ===============================
// 📊 PANEL DEBUG PLAYER
// ===============================

// Gambar panel transparan di pojok kanan bawah berisi info statistik player.
// Hanya muncul jika Config::enableDebugPlayerInfo bernilai true.
void DrawPlayerInfoPanel(const PlayerProfile& profile) {
    int panelWidth = 320;
    int panelHeight = 150;
    int padding = 15;
    int x = Config::screenWidth - panelWidth - 20;
    int y = Config::screenHeight - panelHeight - 20;

    // Background panel transparan
    DrawRectangle(x - 10, y - 10, panelWidth + 20, panelHeight + 20, Color{0, 0, 0, 180});
    DrawRectangleLines(x - 10, y - 10, panelWidth + 20, panelHeight + 20, Color{0, 255, 200, 200});

    int fontSize = 16;
    int lineHeight = 24;
    int curY = y + 5;

    // Tampilkan data profil player baris per baris
    DrawText(TextFormat("Username: %s", profile.username.c_str()), x, curY, fontSize, WHITE);
    curY += lineHeight;
    DrawText(TextFormat("Highest Score: %d", profile.highest_score), x, curY, fontSize, WHITE);
    curY += lineHeight;
    DrawText(TextFormat("Research Points: %d", profile.research_point), x, curY, fontSize, WHITE);
    curY += lineHeight;
    DrawText(TextFormat("Unlocked Words: %zu", profile.unlocked_words.size()), x, curY, fontSize, WHITE);
    curY += lineHeight;
    DrawText(TextFormat("Unlocked Skills: %zu", profile.unlocked_skills.size()), x, curY, fontSize, WHITE);
}
