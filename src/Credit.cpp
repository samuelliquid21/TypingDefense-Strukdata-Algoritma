#include "Credit.h"

// ===============================
// 📜 CREDIT SCREEN
// ===============================

// Buat daftar entry credit default untuk ditampilkan saat scrolling
static std::vector<CreditEntry> createDefaultEntries() {
    return {
    // Opening
    {"", "Thank You For Playing!", true},
    {"", "", false},
    {"", "Cosmic Keypad", true},
    {"", "", false},
    {"", "A Game By:", false},
    {"", "Kelompok 4", true},
    {"", "", false},

    // Engineering Section
    {"", "=== ENGINEERING ===", true},
    {"Gameplay Programmer", "Abiyu", false},
    {"Asteroid System", "Abiyu", false},
    {"Skill System", "Abiyu", false},
    {"Tech Tree & Dictionary", "Abiyu", false},
    {"Login & Register System", "Abiyu & Ahya Mujahid", false},
    {"Skin System & Gacha", "Sardenggan ", false},
    {"RP & Icon System", "Sardenggan", false},
    {"Singly Linked List Module", "Abiyu", false},
    {"Asset & Audio Manager", "Abiyu", false},
    {"Data Manager", "Abiyu", false},
    {"Code Cleanup", "Andaru", false},
    {"", "", false},

    // Design Section
    {"", "=== DESIGN ===", true},
    {"Lead Designer", "Abiyu", false},
    {"Opening Scene Creator", "Sardenggan", false},
    {"Opening Scene Image Creator", "Andaru", false},
    {"Opening Scene Image Creator", "Given", false},
    {"Loading Screen Creator", "Sardenggan ", false},
    {"Game Over Designer", "Given Elyada", false},
    {"Game Pause Designer", "Ahya Mujahid", false},
    {"Visual Effects Design", "Sardenggan ", false},
    {"Credit Designer", "Andaru", false},
    {"Leaderboard Designer", "Given Elyada", false},
    {"", "", false},

    // Art Section
    {"", "=== ART ===", true},
    {"Background Artist", "Abiyu & Andaru", false},
    {"VFX Artist", "Sardenggan ", false},
    {"Video Background", "Sardenggan ", false},
    {"Slideshow & Assets", "Sardenggan & Andaru", false},
    {"", "", false},

    // Audio Section
    {"", "=== AUDIO ===", true},
    {"Sound Designer", "Sardenggan Samuel Gani Munte", false},
    {"Music Composer", "Sardenggan Samuel Gani Munte", false},
    {"Audio Engineer", "Sardenggan Samuel Gani Munte", false},
    {"", "", false},

    // Special Thanks
    {"", "=== SPECIAL THANKS ===", true},
    {"", "Raylib Community", false},
    {"", "Our Friend Informatikans Class B in YUPIEN", false},
    {"", "Everyone Who Played", false},
    {"", "", false},

    // Closing
    {"", "=== THANK YOU FOR PLAYING ===", true},
    {"", "Cosmic Keypad  2026", false},
    {"", "", false},
    {"", "", false},
};
}

Credit::Credit() {
    scrollY = 720.0f;     // Mulai dari bawah layar (efek rolling credit)
    scrollSpeed = 60.0f;
    entries = createDefaultEntries();
    // Hitung total tinggi scroll agar bisa loop dengan benar
    int rowHeight = 45;
    totalHeight = entries.size() * rowHeight + 720.0f; // +720 agar habis scroll ada jeda
}

void Credit::Reset() {
    scrollY = 720.0f;
}

void Credit::Update(bool& backToMenu) {
    // Auto scroll ke atas dengan kecepatan tetap
    scrollY -= scrollSpeed * GetFrameTime();

    // Manual scroll: tahan DOWN untuk mempercepat, UP untuk mundur
    if (IsKeyDown(KEY_DOWN)) scrollY -= 200.0f * GetFrameTime();
    if (IsKeyDown(KEY_UP))   scrollY += 200.0f * GetFrameTime();

    // Jika scroll melewati batas bawah, loop kembali ke awal
    if (scrollY < -totalHeight + 720.0f) {
        scrollY = 720.0f;
    }
    // Batas atas: tidak bisa scroll melebihi posisi awal
    if (scrollY > 720.0f) scrollY = 720.0f;

    // ESC atau BACKSPACE untuk kembali ke menu utama
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE)) {
        backToMenu = true;
    }
}

// Render satu entry credit: header, sub-text tanpa role, atau entry dengan role+nama
static void drawCreditEntry(const CreditEntry& entry, int screenW, float& y, int rowHeight,
                            int headerSize, int nameSize, int roleSize) {
    // Baris kosong hanya memberi jarak vertikal
    if (entry.name.empty() && entry.role.empty()) {
        y += rowHeight;
        return;
    }

    if (entry.isHeader) {
        // Section header: teks besar warna kuning, di tengah
        int w = MeasureText(entry.name.c_str(), headerSize);
        DrawText(entry.name.c_str(), (screenW - w) / 2, (int)y, headerSize, YELLOW);
    } else if (entry.role.empty()) {
        // Sub-text tanpa role (seperti "A Game By:"): di tengah, warna terang
        int w = MeasureText(entry.name.c_str(), nameSize);
        DrawText(entry.name.c_str(), (screenW - w) / 2, (int)y, nameSize, LIGHTGRAY);
    } else {
        // Entry dengan role: role di kiri-tengah, nama di kanan-tengah
        int col1X = screenW / 2 - 20;
        int col2X = screenW / 2 + 20;

        // Nama di kolom kiri (rata kanan)
        int nameW = MeasureText(entry.name.c_str(), nameSize);
        DrawText(entry.name.c_str(), col1X - nameW, (int)y, nameSize, WHITE);

        // Role di kolom kanan (rata kiri) dengan warna oranye
        DrawText(entry.role.c_str(), col2X, (int)y, roleSize, ORANGE);
    }

    y += rowHeight;
}

// Render semua credit entries dengan scrolling
static void drawCreditEntries(const std::vector<CreditEntry>& entries, float scrollY,
                              int screenW, int rowHeight, int headerSize, int nameSize, int roleSize) {
    float y = scrollY;
    for (auto& entry : entries) {
        drawCreditEntry(entry, screenW, y, rowHeight, headerSize, nameSize, roleSize);
    }
}

// Petunjuk navigasi di bagian bawah layar
static void drawCreditHints(int screenW) {
    DrawText("ESC / BACKSPACE : Back to Menu", 20, 690, 18, GRAY);
    DrawText("UP / DOWN : Scroll", screenW - 220, 690, 18, GRAY);
}

void Credit::Draw() {
    int screenW = 1080;
    int rowHeight = 45;
    int headerSize = 28;  // Font size untuk header section
    int nameSize = 22;    // Font size untuk nama/sub-text
    int roleSize = 22;    // Font size untuk role

    drawCreditEntries(entries, scrollY, screenW, rowHeight, headerSize, nameSize, roleSize);

    drawCreditHints(screenW);
}
