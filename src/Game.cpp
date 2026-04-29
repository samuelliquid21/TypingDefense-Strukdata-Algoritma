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

Game::Game() {
    InitWindow(1080, 720, "myWindow");
    InitAudioDevice();
    SetTargetFPS(60);
    SetExitKey(KEY_F12);

    state = GameState::MENU; 
    score = 0;
    statusMenuQuit = false;

    bg.Load("./assets/img/Space_Background.png", 20.0f);
    LeaderboardSystem::Init();
}

// ===============================
// 🔄 CORE LOOP SYSTEM
// ===============================

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

}

void Game::DrawGameplay() {
    DrawText("ini gameplay", 0, 0, 20, WHITE);
}

void Game::UpdateGameOver() {

}

void Game::DrawGameOver() {
    DrawText("ini game over", 0, 0, 20, WHITE);
}

void Game::UpdatePause() {

}

void Game::DrawPause() {
    DrawText("ini pause", 0, 0, 20, WHITE);
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