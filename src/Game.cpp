#include "Game.h"
#include "Leaderboard.h"
#include "Credit.h"
#include "raylib.h"
#include "rlgl.h"

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
    UnloadMusicStream(musicLobby);
    UnloadMusicStream(musicCredit);
    UnloadSound(glitchMasuk);
    UnloadSound(glitchKeluar);
    CloseAudioDevice();
    CloseWindow();
}

// ===============================
// 🌍 GLOBAL INITIALIZATION
// ===============================

Game::Game() : gameplayManager(new GameplayManager()) {
    InitWindow(1080, 720, "Cosmic Keypad - Kelompok 4");
    InitAudioDevice();
    SetTargetFPS(60);
    SetExitKey(KEY_F12);

    // --- SETUP MUSIK LOBBY ---
    musicLobby = LoadMusicStream("assets/sound/soundtrack.mp3");
    SetMusicVolume(musicLobby, 0.5f);
    PlayMusicStream(musicLobby);
    SeekMusicStream(musicLobby, 5.0f); // SKIP 5 DETIK AWAL BIAR LANGSUNG RAME

    state = GameState::MENU; 
    score = 0;
    statusMenuQuit = false;
    isSaved = false;
    highestScore = 0;
    playerName = "Player";

    bg.Load("./assets/img/Space_Background.png", 20.0f);
    gameplayManager->textureInit();
    
    LeaderboardSystem::Init();

    // --- SETUP MUSIK CREDIT ---
    musicCredit = LoadMusicStream("assets/sound/cosmic.mp3");
    SetMusicVolume(musicCredit, 0.5f);

    glitchMasuk = LoadSound("assets/sound/glitchmasuk.mp3");
    glitchKeluar = LoadSound("assets/sound/glitchkeluar.mp3");
    
    transitionTimer = 0.0f;
    isTransitioning = false;
    glitchIntensity = 0.0f;
}

Game::~Game() {
    delete gameplayManager;
    UnloadMusicStream(musicLobby);
    UnloadMusicStream(musicCredit);
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
    float dt = GetFrameTime();

    if (isTransitioning) {
        transitionTimer -= dt;
        glitchIntensity = transitionTimer / 0.6f; 

        if (transitionTimer <= 0) {
            state = targetState; 
            isTransitioning = false;
            glitchIntensity = 0.0f; 
        }
        return; 
    }

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
    
    if (isTransitioning) {
        ClearBackground(BLACK);
        float intensity = transitionTimer / 0.6f; 
        bg.Draw();

        if (intensity > 0.01f) {
            for (int i = 0; i < 12; i++) {
                int y = GetRandomValue(0, 720);
                int h = GetRandomValue(5, 25); 
                
                // Cyan & Pink/Magenta Glitch (Identik Leaderboard)
                Color glitchColor = (i % 2 == 0) ? 
                    (Color){ 0, 255, 200, (unsigned char)(intensity * 180) } : 
                    (Color){ 255, 50, 120, (unsigned char)(intensity * 140) };

                DrawRectangle(0, y, 1080, h, glitchColor);
            }
            
            if (GetRandomValue(0, 10) > 7) {
                DrawRectangle(0, GetRandomValue(0, 720), 1080, GetRandomValue(1, 3), {255, 255, 255, 100});
            }
        }
        
    } else {
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
    }

    EndDrawing();
}

// ===============================
// 🧠 GAME STATE LOGIC
// ===============================

void Game::UpdateMenu() {
    UpdateMusicStream(musicLobby);
    
    if (!IsMusicStreamPlaying(musicLobby)) {
        PlayMusicStream(musicLobby);
        SeekMusicStream(musicLobby, 5.0f); // Re-seek kalo lagu looping
    }

    mainMenu.Update();

    if (mainMenu.IsOptionChosen()) {
        int choice = mainMenu.GetSelectedIndex();
        
        if (choice == 0) { 
            StopMusicStream(musicLobby);
            restartGame();
            state = GameState::GAMEPLAY;
        } 
        else if (choice == 1) { 
            StopMusicStream(musicLobby);
            LeaderboardSystem::Init(); 
            state = GameState::LEADERBOARD;
        } 
        else if (choice == 2) { 
            StopMusicStream(musicLobby);
            if (!IsSoundPlaying(glitchMasuk)) PlaySound(glitchMasuk);
            isTransitioning = true;
            transitionTimer = 0.6f; 
            targetState = GameState::CREDIT;
        } 
        else if (choice == 3) { 
            statusMenuQuit = true;
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
    gameplayManager->draw();
}

void Game::UpdateGameOver() {
    if (!isSaved) {
        auto j = ScoreManager::Load("./data/data.json");
        ScoreManager::InsertOrUpdate(j, playerName, score);
        ScoreManager::Save("./data/data.json", j);
        highestScore = ScoreManager::GetHighest(j);
        isSaved = true;
    }

    if (IsKeyPressed(KEY_ENTER)) {
        state = GameState::MENU;
    }
}

void Game::DrawGameOver() {
    DrawText("GAME OVER", 400, 100, 40, RED);
    DrawText(TextFormat("Score Kamu: %d", score), 400, 200, 20, WHITE);
    DrawText(TextFormat("Skor Tertinggi: %d", highestScore), 400, 240, 20, GREEN);
    DrawText("Tekan ENTER untuk kembali", 400, 300, 20, GRAY);
}

void Game::UpdatePause() {
    pauseMenu.Update();
    
    if (pauseMenu.IsOptionChosen()) {
        if (pauseMenu.GetSelectedIndex() == 0) {
            pauseMenu.StartCountdown();
        } else if (pauseMenu.GetSelectedIndex() == 1) {
            pauseMenu.Reset();
            restartGame();
            state = GameState::MENU;
        }
    }
    
    if (pauseMenu.IsCountingDown() && pauseMenu.IsCountdownFinished()) {
        pauseMenu.Reset();
        state = GameState::GAMEPLAY;
    }
}

void Game::DrawPause() {
    gameplayManager->draw();
    pauseMenu.Draw();
}

void Game::UpdateLeaderboard() {
    bool kembali = false; 
    LeaderboardSystem::Update(kembali);
    if (kembali) state = GameState::MENU;
}

void Game::DrawLeaderboard() {
    LeaderboardSystem::Draw();
}

void Game::UpdateCredit() {
    UpdateMusicStream(musicCredit);
    if (!IsMusicStreamPlaying(musicCredit)) {
        PlayMusicStream(musicCredit);
        SeekMusicStream(musicCredit, 5.0f); // Langsung skip intro credit 5 detik
    }

    bool backToMenu = false;
    creditScreen.Update(backToMenu);
    
    if (backToMenu && !isTransitioning) {
        StopMusicStream(musicCredit);
        if (!IsSoundPlaying(glitchKeluar)) PlaySound(glitchKeluar);
        isTransitioning = true;
        transitionTimer = 0.6f; 
        targetState = GameState::MENU;
    }
}

void Game::DrawCredit() {
    creditScreen.Draw();
}