#include "Game.h"
#include "AudioManager.h"
#include "AssetManager.h"
#include "Leaderboard.h"
#include "Credit.h"
#include "GameConfig.h"
#include "raylib.h"


// ===============================
// 🎮 GAME ENTRY POINT
// ===============================

void Game::Run() {
    // Loop utama: berjalan selama window tidak ditutup dan player belum pilih quit
    while (!WindowShouldClose() && !statusMenuQuit) {
        Update();
        Draw();
    }

    // Bersihkan resource sebelum keluar
    bg.Unload();
    LeaderboardSystem::Unload();
    AssetManager::getInstance().unloadAll();
    CloseAudioDevice();
    CloseWindow();
}

// ===============================
// 🌍 GLOBAL INITIALIZATION
// ===============================

Game::Game() : gameplayManager(new GameplayManager()), techTreeUI(techTree) {
    InitWindow(1080, 720, "Cosmic Keypad - Kelompok 4");

    InitAudioDevice();
    AudioManager::getInstance().Init();
    
    SetTargetFPS(60);

    SetExitKey(KEY_F12);         // F12 untuk force close (debug)

    state = GameState::MENU;
    score = 0;
    statusMenuQuit = false;

    DataManager::getInstance().load();

    techTree.loadFromProfile(m_currentPlayer);

    AssetManager::getInstance().registerSpriteSheet("ship", "./assets/img/Spaceships.png", 5, 3);

    bg.Load("./assets/img/Space_Background.png", 20.0f);
    gameplayManager->textureInit();

    setupCallbacks();

    LeaderboardSystem::Init();
}

Game::~Game() {
    delete gameplayManager;
}

void Game::restartGame() {
    auto* newMgr = new GameplayManager();
    delete gameplayManager;
    gameplayManager = newMgr;
    gameplayManager->textureInit();
    setupCallbacks();
}

void Game::setupCallbacks() {
    gameplayManager->SetAsteroidDestroyedCallback(
        [this](const std::string& word) {
            for (const auto& w : m_currentPlayer.unlocked_words)
                if (w == word) return;
            m_currentPlayer.unlocked_words.push_back(word);
            m_currentPlayer.research_point += 10;
        }
    );
}

// ===============================
// 🔄 CORE LOOP SYSTEM
// ===============================

void Game::Update() {
    float dt = GetFrameTime();

    if (m_transitionEffect.IsActive()) {
        m_transitionEffect.Update(dt);
        if (!m_transitionEffect.IsActive())
            state = m_transitionEffect.GetTargetState();
        return;
    }

    // Background tetap bergerak kecuali saat pause
    if (state != GameState::PAUSE) {
        bg.Update();
    }

    // Delegasikan update ke method sesuai state aktif
    switch (state) {
        case GameState::MENU:         UpdateMenu(); break;
        case GameState::GAMEPLAY:     UpdateGameplay(); break;
        case GameState::PAUSE:        UpdatePause(); break;
        case GameState::GAME_OVER:    UpdateGameOver(); break;
        case GameState::LEADERBOARD:       UpdateLeaderboard(); break;
        case GameState::CREDIT:            UpdateCredit(); break;
        case GameState::LOGIN_AND_REGISTER: UpdateLoginRegister(); break;
        case GameState::REGISTER:          UpdateRegister(); break;
        case GameState::LOGOUT:            UpdateLogout(); break;
        case GameState::UNLOCK_SKILL:      UpdateTechTree(); break;
        case GameState::WORD_DICTIONARY:   UpdateDictionary(); break;
        case GameState::UNLOCKED_WORDS:    UpdateUnlockedWords(); break;
        default: break;
    }
}

void Game::Draw() {
    BeginDrawing();

    if (m_transitionEffect.IsActive()) {
        ClearBackground(BLACK);
        bg.Draw();
        m_transitionEffect.Draw();
    } else {
        bg.Draw();
        // Delegasikan draw ke method sesuai state aktif
        switch (state) {
            case GameState::MENU:         DrawMenu(); break;
            case GameState::GAMEPLAY:     DrawGameplay(); break;
            case GameState::PAUSE:        DrawPause(); break;
            case GameState::GAME_OVER:    DrawGameOver(); break;
            case GameState::LEADERBOARD:       DrawLeaderboard(); break;
            case GameState::CREDIT:            DrawCredit(); break;
            case GameState::LOGIN_AND_REGISTER: DrawLoginRegister(); break;
            case GameState::REGISTER:          DrawRegister(); break;
            case GameState::LOGOUT:            DrawLogout(); break;
            case GameState::UNLOCK_SKILL:      DrawTechTree(); break;
            case GameState::WORD_DICTIONARY:   DrawDictionary(); break;
            case GameState::UNLOCKED_WORDS:    DrawUnlockedWords(); break;
            default: break;
        }
    }

    EndDrawing();
}

// ===============================
// 🧠 GAME STATE LOGIC
// ===============================

void Game::UpdateMenu() {
    if (!m_isLoggedIn) {
        state = GameState::LOGIN_AND_REGISTER;
        return;
    }

    AudioManager::getInstance().UpdateLobby();

    mainMenu.Update();

    // Tombol F1 untuk membuka dictionary kapan saja dari menu
    if (IsKeyPressed(KEY_F1)) {
        m_dictionary.Reset();
        state = GameState::WORD_DICTIONARY;
    }

    // Tangani pilihan menu
    if (mainMenu.IsOptionChosen()) {
        int choice = mainMenu.GetSelectedIndex();

        if (choice == 0) {
            AudioManager::getInstance().StopLobby();
            gameOver.Reset();
            m_sessionBackup = m_currentPlayer;  // Backup sebelum gameplay
            restartGame();
            state = GameState::GAMEPLAY;
        }
        else if (choice == 1) {
            AudioManager::getInstance().StopLobby();
            LeaderboardSystem::Init();
            state = GameState::LEADERBOARD;
        }
        else if (choice == 2) {
            m_transitionEffect.PlaySoundIn();
            m_transitionEffect.Start(GameState::UNLOCK_SKILL);
        }
        else if (choice == 3) {
            m_unlockedWords.BuildFromPlayer(m_currentPlayer);
            m_transitionEffect.PlaySoundIn();
            m_transitionEffect.Start(GameState::UNLOCKED_WORDS);
        }
        else if (choice == 4) {
            AudioManager::getInstance().StopLobby();
            m_transitionEffect.PlaySoundIn();
            m_transitionEffect.Start(GameState::CREDIT);
        }
        else if (choice == 5) {
            logoutScreen.Reset();
            m_transitionEffect.PlaySoundIn();
            m_transitionEffect.Start(GameState::LOGOUT);
        }
        else if (choice == 6) {
            // Keluar dari game
            statusMenuQuit = true;
        }
        mainMenu.ResetChoice();
    }
}

void Game::DrawMenu() {
    mainMenu.Draw();

    if (m_isLoggedIn) {
        std::string greeting = "Hello, " + m_currentPlayer.username + "!";
        int fontSize = 24;
        int textWidth = MeasureText(greeting.c_str(), fontSize);

        int posX = GetScreenWidth() / 2 - textWidth / 2;
        int posY = 220;
        DrawText(greeting.c_str(), posX, posY, fontSize, WHITE);
    }

    if (m_isLoggedIn && Config::enableDebugPlayerInfo) {
        DrawPlayerInfoPanel(m_currentPlayer);
    }
}

void Game::UpdateGameplay() {
    gameplayManager->update(GetFrameTime());

    // Cek apakah player terkena asteroid
    if (gameplayManager->isHit()) {
        score = gameplayManager->score;
        gameOver.SetScore(score);
        if (m_isLoggedIn) {
            int earnedRP = score / 100;
            if (earnedRP > 0)
                m_currentPlayer.research_point += earnedRP;
            if (score > m_currentPlayer.highest_score) {
                m_currentPlayer.highest_score = score;
                m_currentPlayer.survival_time    = gameplayManager->survivalTime;
            }
            DataManager::getInstance().SavePlayer(m_currentPlayer);
        }
        restartGame();          // Reset gameplay untuk sesi berikutnya
        state = GameState::GAME_OVER;
    }

    // ESC untuk pause
    if (IsKeyPressed(KEY_ESCAPE)) {
        state = GameState::PAUSE;
    }
}

void Game::DrawGameplay() {
    gameplayManager->draw();
}

void Game::UpdatePause() {
    pauseMenu.Update();

    if (pauseMenu.IsOptionChosen()) {
        if (pauseMenu.GetSelectedIndex() == 0) {
            pauseMenu.StartCountdown(); // Lanjutkan game dengan countdown
        } else if (pauseMenu.GetSelectedIndex() == 1) {
            // Kembali ke menu — undo semua perubahan session
            pauseMenu.Reset();
            m_currentPlayer = m_sessionBackup;
            restartGame();
            state = GameState::MENU;
        }
    }

    // Setelah countdown selesai, kembali ke gameplay
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

// ===============================
// LEADERBOARD STATE
// ===============================

void Game::UpdateLeaderboard() {
    bool kembali = false;
    LeaderboardSystem::Update(kembali);
    if (kembali) {
        state = GameState::MENU;
    }
}

void Game::DrawLeaderboard() {
    LeaderboardSystem::Draw();
}

// ===============================
// CREDIT STATE
// ===============================

void Game::UpdateCredit() {
    AudioManager::getInstance().UpdateCredit();

    bool backToMenu = false;
    creditScreen.Update(backToMenu);

    if (backToMenu && !m_transitionEffect.IsActive()) {
        AudioManager::getInstance().StopCredit();
        m_transitionEffect.PlaySoundOut();
        m_transitionEffect.Start(GameState::MENU);
    }
}

void Game::DrawCredit() {
    creditScreen.Draw();
}

// ===============================
// LOGIN / LOGOUT (BELUM IMPLEMENTASI)
// ===============================

void Game::UpdateLoginRegister() {
    AudioManager::getInstance().UpdateLobbyNoSeek();

    loginScreen.Update();
    if (loginScreen.ShouldLogin()) {
        m_currentPlayer = loginScreen.GetProfile();
        techTree.loadFromProfile(m_currentPlayer);
        m_isLoggedIn = true;
        loginScreen.Reset();
        state = GameState::MENU;
    } else if (loginScreen.ShouldGoToRegister()) {
        registerScreen.Reset();
        state = GameState::REGISTER;
    } else if (loginScreen.ShouldGoBack()) {
        loginScreen.Reset();
        statusMenuQuit = true;
    }
}

void Game::DrawLoginRegister() {
    bg.Draw();
    loginScreen.Draw();
}

void Game::UpdateRegister() {
    AudioManager::getInstance().UpdateLobbyNoSeek();

    registerScreen.Update();

    if (registerScreen.IsRegisterSuccess()) {
        DataManager::getInstance().FindPlayer(registerScreen.GetRegisteredUsername(), m_currentPlayer);
        techTree.loadFromProfile(m_currentPlayer);
        m_isLoggedIn = true;
        registerScreen.Reset();
        state = GameState::MENU;
    } 
    else if (registerScreen.ShouldGoToLogin()) {
        loginScreen.Reset();
        state = GameState::LOGIN_AND_REGISTER;
    }
}

void Game::DrawRegister() {
    bg.Draw();
    registerScreen.Draw();
}

void Game::UpdateLogout() {
    logoutScreen.Update();

    if (logoutScreen.IsFinished()) {
        m_currentPlayer = PlayerProfile{};
        m_isLoggedIn = false;
        techTree.loadFromProfile(m_currentPlayer);
        logoutScreen.Reset();
        state = GameState::LOGIN_AND_REGISTER;
    }
}

void Game::DrawLogout() {
    bg.Draw();
    DrawText("Logout berhasil!", 400, 300, 30, WHITE);
    DrawText("Kembali ke menu...", 380, 350, 20, LIGHTGRAY);
}

// ===============================
// TECH TREE / UNLOCK SKILL
// ===============================

void Game::UpdateTechTree() {
    techTreeUI.Update();

    // Proses klik untuk membeli skill di tech tree
    Vector2 mousePos = GetMousePosition();
    bool clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    if (techTree.handleInput(mousePos, clicked, m_currentPlayer)) {
        // Jika ada perubahan, simpan profile pemain
        DataManager::getInstance().SavePlayer(m_currentPlayer);
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        m_transitionEffect.PlaySoundOut();
        m_transitionEffect.Start(GameState::MENU);
    }
}

void Game::DrawTechTree() {
    techTreeUI.Draw(m_currentPlayer);
}

// ===============================
// WORD DICTIONARY
// ===============================

void Game::UpdateDictionary() {
    m_dictionary.Update();
    if (m_dictionary.WantsToGoBack()) {
        m_transitionEffect.PlaySoundOut();
        m_transitionEffect.Start(GameState::MENU);
    }
}

void Game::DrawDictionary() {
    m_dictionary.Draw();
}

// ===============================
// UNLOCKED WORDS
// ===============================

void Game::UpdateUnlockedWords() {
    m_unlockedWords.Update();
    if (m_unlockedWords.WantsToGoBack()) {
        m_transitionEffect.PlaySoundOut();
        m_transitionEffect.Start(GameState::MENU);
    }
}

void Game::DrawUnlockedWords() {
    m_unlockedWords.Draw();
}


