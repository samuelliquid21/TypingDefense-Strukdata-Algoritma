#include "Game.h"
#include "Leaderboard.h"

// ===============================
// 🎮 GAME ENTRY POINT
// ===============================

void Game::Run() {
    while (!WindowShouldClose() && !statusMenuQuit) {
        Update();
        Draw();
    }

    bg.Unload();
    LeaderboardSystem::Unload();
    CloseAudioDevice();
    CloseWindow();
}

// ===============================
// 🌍 GLOBAL INITIALIZATION
// ===============================

Game::Game() : gameplayManager(new GameplayManager()) {
    InitWindow(1080, 720, "myWindow");
    InitAudioDevice();
    SetTargetFPS(60);
    SetExitKey(KEY_F12);

    state = GameState::MENU; 
    score = 0;
    statusMenuQuit = false;

    bg.Load("./assets/img/Space_Background.png", 20.0f);
    gameplayManager->textureInit();
    LeaderboardSystem::Init();
}

Game::~Game() {
    delete gameplayManager;
}

void Game::restartGame() {
    delete gameplayManager;
    gameplayManager = new GameplayManager();
    gameplayManager->textureInit();
}

// ===============================
// 🔄 CORE LOOP SYSTEM
// ===============================

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
}

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
}

// ===============================
// 🧠 GAME STATE LOGIC
// ===============================

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
}

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

void Game::UpdateLeaderboard() {
    static bool firstTime = true;
    if (firstTime) {
        LeaderboardSystem::Init();
        firstTime = false;
    }
    
    bool kembali = false; 
    LeaderboardSystem::Update(kembali);
    
    if (kembali) {
        firstTime = true;  // Reset buat next time
        state = GameState::MENU;
    }
}

void Game::DrawLeaderboard() {
    LeaderboardSystem::Draw();
}

void Game::UpdateCredit() {

}

void Game::DrawCredit() {
    DrawText("ini credit", 0, 0, 20, WHITE);
}