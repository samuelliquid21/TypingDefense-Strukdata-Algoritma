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

Game::Game() : gameplayManager(new GameplayManager()) {
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

Game::~Game() {
    delete gameplayManager;
}

void Game::restartGame() {
    delete gameplayManager;
    gameplayManager = new GameplayManager();
}

// ===============================
// 🔄 CORE LOOP SYSTEM
// ===============================

// Update semua logic (TIDAK BOLEH ADA DRAW DI SINI)
void Game::Update() {
    ClearBackground(BLACK);
    
    // Update background during all states except pause
    if (state != GameState::PAUSE) {
        bg.Update();
    }

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
                restartGame();
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
    gameplayManager->update(GetFrameTime());
    if (gameplayManager->isHit()) {
        restartGame();
        state = GameState::GAME_OVER;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        state = GameState::PAUSE;
    }
}
void Game::DrawGameplay() {
    DrawText("ini gameplay", 0, 0, 20, WHITE);
    gameplayManager->draw();
}

// ===== GAME OVER =====
void Game::UpdateGameOver() {
    // 📌 Ambil username dan score, tambahkan datanya ke file json (folder data), kembali ke menu
    if (IsKeyPressed(KEY_ESCAPE)) {
        state = GameState::MENU;
    }
}
void Game::DrawGameOver() {
    DrawText("ini game over", 0, 0, 20, WHITE);
    // 📌 Tampilkan score akhir, perlihatkan input nama
}

// ===== PAUSE =====
void Game::UpdatePause() {
    pauseMenu.Update();
    
    if (pauseMenu.IsOptionChosen()) {
        if (pauseMenu.GetSelectedIndex() == 0) {
            // Resume Game - Start countdown
            pauseMenu.StartCountdown();
        } else if (pauseMenu.GetSelectedIndex() == 1) {
            // Back to Menu - Restart game
            pauseMenu.Reset();
            restartGame();
            state = GameState::MENU;
        }
    }
    
    // Handle countdown completion
    if (pauseMenu.IsCountingDown() && pauseMenu.IsCountdownFinished()) {
        pauseMenu.Reset();
        state = GameState::GAMEPLAY;
    }
}
void Game::DrawPause() {
    // Draw game world first (will show behind semi-transparent overlay)
    gameplayManager->draw();
    
    // Then draw pause menu on top
    pauseMenu.Draw();
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