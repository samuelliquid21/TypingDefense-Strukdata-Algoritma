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
                gameOver.Reset();
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

// MENU STATE

void Game::DrawMenu() {
    mainMenu.Draw();
}

void Game::UpdateGameplay() {
    gameplayManager->update(GetFrameTime());
    if (gameplayManager->isHit()) {
        score = gameplayManager->score;
        gameOver.SetScore(score);
        restartGame();
        state = GameState::GAME_OVER;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        state = GameState::PAUSE;
    }
}

// GAMEPLAY, PAUSE, AND GAME OVER STATE

void Game::DrawGameplay() {
    DrawText("ini gameplay", 0, 0, 20, WHITE);
    gameplayManager->draw();
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
    gameplayManager->draw();
    pauseMenu.Draw();
}

void Game::UpdateGameOver() {
    gameOver.Update();
    if (gameOver.ShouldReturnToMenu()) {
        state = GameState::MENU;
    }
}

void Game::DrawGameOver() {
    gameOver.Draw();
}

// LEADERBOARD STATE

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

// CREDIT STATE

void Game::UpdateCredit() {

}

void Game::DrawCredit() {
    DrawText("ini credit", 0, 0, 20, WHITE);
}