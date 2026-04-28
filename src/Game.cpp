#include "Game.h"

// ===============================
// 🎮 GAME ENTRY POINT
// ===============================

// Menjalankan Game Loop utama
void Game::Run() {
    // 🔁 MAIN LOOP (JANGAN TARO LOGIKA BERAT DISINI)
    while (!WindowShouldClose() && !statusMenuQuit) {
        Update();   // Update logic
        Draw();     // Render
    }

    // 🧹 Cleanup (resource release)
    bg.Unload();
    CloseWindow();
}

// ===============================
// 🌍 GLOBAL INITIALIZATION
// ===============================

Game::Game() {
    InitWindow(1080, 720, "myWindow");
    SetTargetFPS(60);
    SetExitKey(KEY_F12);

    // 🎯 Default State
    // STATE: MENU, GAMEPLAY, PAUSE, GAME_OVER, LEADERBOARD, CREDIT
    state = GameState::MENU; 
    score = 0;
    statusMenuQuit = false;

    // 📌 TEAM NOTE:
    // - Tambahkan semua inisialisasi global di sini
    // - Contoh: load texture, audio, font, config
    bg.Load("./assets/img/Space_Background.png", 20.0f);
}

// ===============================
// 🔄 CORE LOOP SYSTEM
// ===============================

// Update semua logic (TIDAK BOLEH ADA DRAW DI SINI)
void Game::Update() {
    ClearBackground(BLACK);
    bg.Update();

    switch (state) {
        case GameState::MENU:         UpdateMenu(); break;
        case GameState::GAMEPLAY:     UpdateGameplay(); break;
        case GameState::PAUSE:        UpdatePause(); break;
        case GameState::GAME_OVER:    UpdateGameOver(); break;
        case GameState::LEADERBOARD:  UpdateLeaderboard(); break;
        case GameState::CREDIT:       UpdateCredit(); break;
        default: break;
    }

    // 📌 TEAM NOTE:
    // - Hanya logika 
    // - Jangan render di sini!
}

// Render semua visual
void Game::Draw() {
    BeginDrawing();

    bg.Draw();

    switch (state) {
        case GameState::MENU:         DrawMenu(); break;
        case GameState::GAMEPLAY:     DrawGameplay(); break;
        case GameState::PAUSE:        DrawPause(); break;
        case GameState::GAME_OVER:    DrawGameOver(); break;
        case GameState::LEADERBOARD:  DrawLeaderboard(); break;
        case GameState::CREDIT:       DrawCredit(); break;
        default: break;
    }

    EndDrawing();

    // 📌 TEAM NOTE:
    // - Hanya untuk rendering
    // - Jangan update logic di sini!
    // - jika ingin render sesuatu pastikan ada di antara begin dan end drawing
}

// ===============================
// 🧠 GAME STATE LOGIC
// ===============================

// ===== MENU =====
void Game::UpdateMenu() {
    mainMenu.Update();

    if (mainMenu.IsOptionChosen()) {
        switch (mainMenu.GetSelectedIndex()) {
            case 0:
                state = GameState::GAMEPLAY;
                break;
            case 1:
                state = GameState::LEADERBOARD;
                break;
            case 2:
                state = GameState::CREDIT;
                break;
            case 3:
                statusMenuQuit = true;
                break;
                
        }
        mainMenu.ResetChoice();
    }
}
void Game::DrawMenu() {
    mainMenu.Draw();
    // 📌 Gambar UI menu
}

// ===== GAMEPLAY =====
void Game::UpdateGameplay() {
    player.update(GetFrameTime());
}
void Game::DrawGameplay() {
    DrawText("ini gameplay", 0, 0, 20, WHITE);
    player.draw();
}

// ===== GAME OVER =====
void Game::UpdateGameOver() {
    // 📌 Ambil username dan score, tambahkan datanya ke file json (folder data), kembali ke menu
}
void Game::DrawGameOver() {
    DrawText("ini game over", 0, 0, 20, WHITE);
    // 📌 Tampilkan score akhir, perlihatkan input nama
}

// ===== PAUSE =====
void Game::UpdatePause() {

}
void Game::DrawPause() {
    DrawText("ini pause", 0, 0, 20, WHITE);
    // 📌 Overlay pause screen
}

// ===== LEADERBOARD =====
void Game::UpdateLeaderboard() {
    // 📌 Scroll / back ke menu, data dari file json yang ada di folder data
}
void Game::DrawLeaderboard() {
    DrawText("ini leaderboard", 0, 0, 20, WHITE);
    // 📌 Tampilkan ranking
}

// ===== CREDIT =====
void Game::UpdateCredit() {
    // 📌 Back ke menu
}
void Game::DrawCredit() {
    DrawText("ini credit", 0, 0, 20, WHITE);
    // 📌 Tampilkan nama anggota
}