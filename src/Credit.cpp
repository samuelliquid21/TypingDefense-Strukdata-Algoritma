#include "Credit.h"

Credit::Credit() {
    scrollY = 720.0f;
    scrollSpeed = 60.0f;

    entries = {
    // Opening
    {"", "Thank You For Playing!", true},
    {"", "", false},
    {"", "Cosmic Keypad", true},
    {"", "", false},
    {"", "A Game By:", false},
    {"", "Kelompok 4", true},
    {"", "", false},

    // Anggota
    {"2510511039", "Sardenggan Samuel Gani Munte", false},
    {"2510511046", "Given Elyada Bani", false},
    {"2510511047", "Ahya Mujahid Almadani", false},
    {"2510511050", "Muhammad Abiyu Aldy", false},
    {"2510511069", "Andaru Rasyid Shiddiq", false},
    {"", "", false},

    // Team Section
    {"", "=== TEAM ===", true},
    {"Director", "orang", false},
    {"Executive Producer", "orang", false},
    {"Co-Producer", "orang", false},
    {"", "", false},

    // Engineering Section
    {"", "=== ENGINEERING ===", true},
    {"Lead Programmer", "orang", false},
    {"Gameplay Programmer", "orang", false},
    {"Engine Programmer", "orang", false},
    {"Systems Programmer", "orang", false},
    {"AI Programmer", "orang", false},
    {"             ", "orang", false},//kalo lebih 1 orang
    {"             ", "orang", false},//kalo lebih 1 orang
    {"             ", "", false},//kalo lebih 1 orang

    // Design Section
    {"", "=== DESIGN ===", true},
    {"Lead Designer", "orang", false},
    {"Game Designer", "orang", false},
    {"Level Designer", "orang", false},
    {"UI/UX Designer", "orang", false},
    {"Credit Designer", "orang", false},
    {"", "", false},

    // Art Section
    {"", "=== ART ===", true},
    {"Art Director", "orang", false},
    {"Lead Artist", "orang", false},
    {"Background Artist", "orang", false},
    {"VFX Artist", "orang", false},
    {"", "", false},

    // Audio Section
    {"", "=== AUDIO ===", true},
    {"Sound Designer", "orang", false},
    {"Music Composer", "orang", false},
    {"Audio Engineer", "orang", false},
    {"", "", false},

    // QA Section
    {"", "=== QUALITY ASSURANCE ===", true},
    {"QA Lead", "orang", false},
    {"QA Tester", "orang", false},
    {"QA Tester", "orang", false},
    {"", "", false},

    // Special Thanks
    {"", "=== SPECIAL THANKS ===", true},
    {"", "Raylib Community", false},
    {"", "Our Families & Friends", false},
    {"", "Everyone Who Played", false},
    {"", "", false},

    // Closing
    {"", "=== THANK YOU FOR PLAYING ===", true},
    {"", "Cosmic Keypad  2026", false},
    {"", "", false},
    {"", "", false},
};

    // hitung total tinggi scroll
    int rowHeight = 45;
    totalHeight = entries.size() * rowHeight + 720.0f;
}

void Credit::Update(bool& backToMenu) {
    // auto scroll
    scrollY -= scrollSpeed * GetFrameTime();

    // manual scroll
    if (IsKeyDown(KEY_DOWN)) scrollY -= 200.0f * GetFrameTime();
    if (IsKeyDown(KEY_UP))   scrollY += 200.0f * GetFrameTime();

    // loop
    if (scrollY < -totalHeight + 720.0f) {
        scrollY = 720.0f;
    }
    // batas atas
    if (scrollY > 720.0f) scrollY = 720.0f;

    // back ke menu
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE)) {
        backToMenu = true;
    }
}

void Credit::Draw() {
    int screenW = 1080;
    int rowHeight = 45;
    int headerSize = 28;
    int nameSize = 22;
    int roleSize = 22;

    float y = scrollY;

    for (auto& entry : entries) {
        if (entry.name.empty() && entry.role.empty()) {
            y += rowHeight;
            continue;
        }

        if (entry.isHeader) {
            // Section header - kuning
            int w = MeasureText(entry.name.c_str(), headerSize);
            DrawText(entry.name.c_str(), (screenW - w) / 2, (int)y, headerSize, YELLOW);
        } else if (entry.role.empty()) {
            // Sub-text centered (kayak "A Game By:")
            int w = MeasureText(entry.name.c_str(), nameSize);
            DrawText(entry.name.c_str(), (screenW - w) / 2, (int)y, nameSize, LIGHTGRAY);
        } else {
            // Role (kiri-tengah) + Nama (kanan-tengah)
            int col1X = screenW / 2 - 20;
            int col2X = screenW / 2 + 20;

            // nama di kiri
            int nameW = MeasureText(entry.name.c_str(), nameSize);
            DrawText(entry.name.c_str(), col1X - nameW, (int)y, nameSize, WHITE);

            // role di kanan - oranye
            DrawText(entry.role.c_str(), col2X, (int)y, roleSize, ORANGE);
        }

        y += rowHeight;
    }

    // petunjuk
    DrawText("ESC / BACKSPACE : Back to Menu", 20, 690, 18, GRAY);
    DrawText("UP / DOWN : Scroll", screenW - 220, 690, 18, GRAY);
}