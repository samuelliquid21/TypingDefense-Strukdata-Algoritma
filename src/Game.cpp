#include "Game.h"
#include "Leaderboard.h"
#include "Credit.h"
#include "GameConfig.h"
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

Game::Game() : gameplayManager(new GameplayManager()), techTreeUI(techTree) {
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

    DataManager::getInstance().load();

    // TODO: Hapus saat login/register sudah ada
    DataManager::getInstance().FindPlayer("hyperion", m_currentPlayer);
    m_isLoggedIn = true;

    techTree.loadFromProfile(m_currentPlayer);

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
        case GameState::LEADERBOARD:       UpdateLeaderboard(); break;
        case GameState::CREDIT:            UpdateCredit(); break;
        case GameState::LOGIN_AND_REGISTER: UpdateLoginRegister(); break;
        case GameState::LOGOUT:            UpdateLogout(); break;
        case GameState::UNLOCK_SKILL:      UpdateTechTree(); break;
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
            case GameState::LEADERBOARD:       DrawLeaderboard(); break;
            case GameState::CREDIT:            DrawCredit(); break;
            case GameState::LOGIN_AND_REGISTER: DrawLoginRegister(); break;
            case GameState::LOGOUT:            DrawLogout(); break;
            case GameState::UNLOCK_SKILL:      DrawTechTree(); break;
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
            gameOver.Reset();
            restartGame();
            state = GameState::GAMEPLAY;
        } 
        else if (choice == 1) { 
            StopMusicStream(musicLobby);
            LeaderboardSystem::Init(); 
            state = GameState::LEADERBOARD;
        } 
        else if (choice == 2) {
            state = GameState::UNLOCK_SKILL;
        }
        else if (choice == 3) { 
            StopMusicStream(musicLobby);
            if (!IsSoundPlaying(glitchMasuk)) PlaySound(glitchMasuk);
            isTransitioning = true;
            transitionTimer = 0.6f; 
            targetState = GameState::CREDIT;
        } 
        else if (choice == 4) { 
            statusMenuQuit = true;
        }
        mainMenu.ResetChoice();
    }
}

void Game::DrawMenu() {
    mainMenu.Draw();
    if (m_isLoggedIn && Config::enableDebugPlayerInfo) {
        DrawPlayerInfo();
    }
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
    gameplayManager->draw();
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

void Game::UpdateGameOver() {
    if (m_isLoggedIn && score > m_currentPlayer.highest_score) {
        m_currentPlayer.highest_score = score;
        DataManager::getInstance().SavePlayer(m_currentPlayer);
    }

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
    bool kembali = false; 
    LeaderboardSystem::Update(kembali);
    if (kembali) state = GameState::MENU;
}

void Game::DrawLeaderboard() {
    LeaderboardSystem::Draw();
}

// CREDIT STATE

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

void Game::UpdateLoginRegister() {}

void Game::DrawLoginRegister() {}

void Game::UpdateLogout() {}

void Game::DrawLogout() {}

void Game::UpdateTechTree() {
    techTreeUI.Update();

    Vector2 mousePos = GetMousePosition();
    bool clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    if (techTree.handleInput(mousePos, clicked, m_currentPlayer)) {
        DataManager::getInstance().SavePlayer(m_currentPlayer);
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        state = GameState::MENU;
    }
}

void Game::DrawTechTree() {
    techTreeUI.Draw(m_currentPlayer);
}

void Game::DrawPlayerInfo() {
    int panelWidth = 320;
    int panelHeight = 150;
    int padding = 15;
    int x = Config::screenWidth - panelWidth - 20;
    int y = Config::screenHeight - panelHeight - 20;

    DrawRectangle(x - 10, y - 10, panelWidth + 20, panelHeight + 20, Color{0, 0, 0, 180});
    DrawRectangleLines(x - 10, y - 10, panelWidth + 20, panelHeight + 20, Color{0, 255, 200, 200});

    int fontSize = 16;
    int lineHeight = 24;
    int curY = y + 5;

    DrawText(TextFormat("Username: %s", m_currentPlayer.username.c_str()), x, curY, fontSize, WHITE);
    curY += lineHeight;
    DrawText(TextFormat("Highest Score: %d", m_currentPlayer.highest_score), x, curY, fontSize, WHITE);
    curY += lineHeight;
    DrawText(TextFormat("Research Points: %d", m_currentPlayer.research_point), x, curY, fontSize, WHITE);
    curY += lineHeight;
    DrawText(TextFormat("Unlocked Words: %zu", m_currentPlayer.unlocked_words.size()), x, curY, fontSize, WHITE);
    curY += lineHeight;
    DrawText(TextFormat("Unlocked Skills: %zu", m_currentPlayer.unlocked_skills.size()), x, curY, fontSize, WHITE);
}