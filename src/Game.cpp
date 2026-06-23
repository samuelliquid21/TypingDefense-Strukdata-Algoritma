#include "Game.h"
#include "AudioManager.h"
#include "AssetManager.h"
#include "Leaderboard.h"
#include "Credit.h"
#include "GameConfig.h"
#include "SkinManager.h"
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

    SkinManager::getInstance().save();

    // Bersihkan resource sebelum keluar
    openingScene.Unload();
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
    initAudioAndWindow();
    initGameStateAndAssets();
}

Game::~Game() {
    delete gameplayManager;
}

// Inisialisasi window, audio, dan pengaturan dasar
void Game::initAudioAndWindow() {
    InitWindow(1080, 720, "Cosmic Keypad - Kelompok 4");

    InitAudioDevice();
    AudioManager::getInstance().Init();
    
    SetTargetFPS(60);

    SetExitKey(KEY_F12);         // F12 untuk force close (debug)
}

// Inisialisasi state game, data player, dan resource assets
void Game::initGameStateAndAssets() {
    state = GameState::LOADING;
    score = 0;
    statusMenuQuit = false;

    DataManager::getInstance().load();

    techTree.loadFromProfile(m_currentPlayer);

    AssetManager::getInstance().registerSpriteSheet("ship", "./assets/img/Spaceships.png", 5, 3);

    bg.Load("./assets/img/Space_Background.png", 20.0f);
    gameplayManager->textureInit();
    gameplayManager->setUnlockedSkills(&m_currentPlayer.unlocked_skills);

    setupCallbacks();

    LeaderboardSystem::Init();
}

void Game::restartGame() {
    auto* newMgr = new GameplayManager();
    delete gameplayManager;
    gameplayManager = newMgr;
    gameplayManager->textureInit();
    gameplayManager->setUnlockedSkills(&m_currentPlayer.unlocked_skills);
    setupCallbacks();
}

void Game::setupCallbacks() {
    gameplayManager->SetAsteroidDestroyedCallback(
        [this](const std::string& word) {
            for (const auto& w : m_currentPlayer.unlocked_words)
                if (w == word) return;
            m_currentPlayer.unlocked_words.push_back(word);
            SkinManager::getInstance().addRP(10);
        }
    );
}

// ===============================
// 🔄 CORE LOOP SYSTEM
// ===============================

void Game::Update() {
    float dt = GetFrameTime();

    m_transitionEffect.Update(dt);

    // Background tetap bergerak kecuali saat pause, loading, atau opening
    if (state != GameState::PAUSE && state != GameState::LOADING && state != GameState::OPENING) {
        bg.Update();
    }

    // Delegasikan update ke method sesuai state aktif
    dispatchUpdateState();
}

// Delegasikan panggilan update ke method state yang sesuai
void Game::dispatchUpdateState() {
    switch (state) {
        case GameState::LOADING:      UpdateLoading(); break;
        case GameState::OPENING:      UpdateOpening(); break;
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
        case GameState::SKIN_SELECT:       UpdateSkinSelect(); break;
        case GameState::GACHA:             UpdateGacha(); break;
        default: break;
    }
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(BLACK);
    if (state != GameState::LOADING && state != GameState::OPENING)
        bg.Draw();

    // Delegasikan draw ke method sesuai state aktif
    dispatchDrawState();

    // Overlay transisi di atas semua konten state
    if (state != GameState::LOADING && state != GameState::OPENING)
        m_transitionEffect.Draw();

    EndDrawing();
}

// Delegasikan panggilan draw ke method state yang sesuai
void Game::dispatchDrawState() {
    switch (state) {
        case GameState::LOADING:      DrawLoading(); break;
        case GameState::OPENING:      DrawOpening(); break;
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
        case GameState::SKIN_SELECT:       DrawSkinSelect(); break;
        case GameState::GACHA:             DrawGacha(); break;
        default: break;
    }
}

// ===============================
// 🧠 GAME STATE LOGIC
// ===============================

void Game::UpdateLoading() {
    bool done = false;
    loadingScreen.Update(done);
    if (done) {
        openingScene.Init();
        state = GameState::OPENING;
    }
}

void Game::DrawLoading() {
    loadingScreen.Draw();
}

void Game::UpdateOpening() {
    bool done = false;
    openingScene.Update(done);
    if (done) {
        openingScene.Unload();
        state = GameState::MENU;
    }
}

void Game::DrawOpening() {
    openingScene.Draw();
}

void Game::UpdateMenu() {
    if (!m_isLoggedIn) {
        state = GameState::LOGIN_AND_REGISTER;
        return;
    }

    AudioManager::getInstance().UpdateLobby();

    mainMenu.Update();

    // Tombol F1 untuk membuka dictionary kapan saja dari menu
    handleMenuF1Key();

    // Tangani pilihan menu
    handleMenuChoice();
}

// Tangani tombol F1 untuk membuka dictionary dari menu
void Game::handleMenuF1Key() {
    if (IsKeyPressed(KEY_F1)) {
        AudioManager::getInstance().StopLobby();
        m_dictionary.Reset();
        state = GameState::WORD_DICTIONARY;
    }
}

// Tangani seluruh pilihan menu utama dengan switch
void Game::handleMenuChoice() {
    if (!mainMenu.IsOptionChosen()) return;
    int choice = mainMenu.GetSelectedIndex();

    switch (choice) {
        case 0: startGameplay(); break;
        case 1: openLeaderboardFromMenu(); break;
        case 2: openSkinSelectFromMenu(); break;
        case 3: openTechTreeFromMenu(); break;
        case 4: openUnlockedWordsFromMenu(); break;
        case 5: openCreditFromMenu(); break;
        case 6: openLogoutFromMenu(); break;
        case 7: quitGame(); break;
    }
    mainMenu.ResetChoice();
}

// Mulai sesi gameplay baru dari menu
void Game::startGameplay() {
    AudioManager::getInstance().StopLobby();
    gameOver.Reset();
    m_sessionBackup = m_currentPlayer;  // Backup sebelum gameplay
    restartGame();
    m_transitionEffect.PlaySoundIn();
    state = GameState::GAMEPLAY;
    m_transitionEffect.Start();
}

// Buka leaderboard dari menu
void Game::openLeaderboardFromMenu() {
    AudioManager::getInstance().StopLobby();
    LeaderboardSystem::Init();
    state = GameState::LEADERBOARD;
}

// Buka skin select dari menu
void Game::openSkinSelectFromMenu() {
    AudioManager::getInstance().StopLobby();
    state = GameState::SKIN_SELECT;
    m_transitionEffect.PlaySoundIn();
    m_transitionEffect.Start();
}

// Buka tech tree skill dari menu
void Game::openTechTreeFromMenu() {
    AudioManager::getInstance().StopLobby();
    state = GameState::UNLOCK_SKILL;
    m_transitionEffect.PlaySoundIn();
    m_transitionEffect.Start();
}

// Buka daftar kata yang sudah di-unlock dari menu
void Game::openUnlockedWordsFromMenu() {
    AudioManager::getInstance().StopLobby();
    m_unlockedWords.BuildFromPlayer(m_currentPlayer);
    state = GameState::UNLOCKED_WORDS;
    m_transitionEffect.PlaySoundIn();
    m_transitionEffect.Start();
}

// Buka credit screen dari menu
void Game::openCreditFromMenu() {
    AudioManager::getInstance().StopLobby();
    creditScreen.Reset();
    state = GameState::CREDIT;
    m_transitionEffect.PlaySoundIn();
    m_transitionEffect.Start();
}

// Buka logout screen dari menu
void Game::openLogoutFromMenu() {
    AudioManager::getInstance().StopLobby();
    logoutScreen.Reset();
    state = GameState::LOGOUT;
    m_transitionEffect.PlaySoundIn();
    m_transitionEffect.Start();
}

// Keluar dari game seluruhnya
void Game::quitGame() {
    // Keluar dari game
    statusMenuQuit = true;
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
    AudioManager::getInstance().UpdateDefault();
    gameplayManager->update(GetFrameTime());

    // Cek apakah player terkena asteroid
    handleGameplayHit();

    // ESC untuk pause
    if (IsKeyPressed(KEY_ESCAPE)) {
        state = GameState::PAUSE;
    }
}

// Tangani tabrakan asteroid dengan player: simpan skor, update profile, transisi ke game over
void Game::handleGameplayHit() {
    if (!gameplayManager->isHit()) return;
    score = gameplayManager->score;
    gameOver.SetScore(score);
    if (m_isLoggedIn) {
        int earnedRP = score / 100;
        if (earnedRP > 0)
            SkinManager::getInstance().addRP(earnedRP);
        if (score > m_currentPlayer.highest_score) {
            m_currentPlayer.highest_score = score;
            m_currentPlayer.survival_time    = gameplayManager->survivalTime;
        }
        DataManager::getInstance().SavePlayer(m_currentPlayer);
        SkinManager::getInstance().save();
    }
    restartGame();          // Reset gameplay untuk sesi berikutnya
    state = GameState::GAME_OVER;
}

void Game::DrawGameplay() {
    gameplayManager->draw();
}

void Game::UpdatePause() {
    AudioManager::getInstance().UpdateDefault();
    pauseMenu.Update();
    handlePauseChoice();

    // Setelah countdown selesai, kembali ke gameplay
    if (pauseMenu.IsCountingDown() && pauseMenu.IsCountdownFinished()) {
        pauseMenu.Reset();
        state = GameState::GAMEPLAY;
    }
}

// Tangani pilihan menu pause: lanjutkan game atau kembali ke menu
void Game::handlePauseChoice() {
    if (!pauseMenu.IsOptionChosen()) return;
    if (pauseMenu.GetSelectedIndex() == 0) {
        pauseMenu.StartCountdown(); // Lanjutkan game dengan countdown
    } else if (pauseMenu.GetSelectedIndex() == 1) {
        // Kembali ke menu — undo semua perubahan session
        AudioManager::getInstance().stopMusic("bgm");
        pauseMenu.Reset();
        m_currentPlayer = m_sessionBackup;
        restartGame();
        state = GameState::MENU;
    }
}

void Game::DrawPause() {
    gameplayManager->draw();
    pauseMenu.Draw();
}

void Game::UpdateGameOver() {
    AudioManager::getInstance().UpdateDefault();
    gameOver.Update();
    if (gameOver.ShouldReturnToMenu()) {
        AudioManager::getInstance().stopMusic("bgm");
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
        state = GameState::MENU;
        m_transitionEffect.PlaySoundOut();
        m_transitionEffect.Start();
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
        SkinManager::getInstance().init(m_currentPlayer.username);
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
        SkinManager::getInstance().init(m_currentPlayer.username);
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
    AudioManager::getInstance().UpdateDefault();
    logoutScreen.Update();

    if (logoutScreen.IsFinished()) {
        AudioManager::getInstance().stopMusic("bgm");
        m_currentPlayer = PlayerProfile{};
        m_isLoggedIn = false;
        SkinManager::getInstance().init();
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
    AudioManager::getInstance().UpdateDefault();
    techTreeUI.Update();

    // Proses klik untuk membeli skill di tech tree
    Vector2 mousePos = GetMousePosition();
    bool clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    if (techTree.handleInput(mousePos, clicked, m_currentPlayer)) {
        // Jika ada perubahan, simpan profile pemain
        DataManager::getInstance().SavePlayer(m_currentPlayer);
        SkinManager::getInstance().save();
    }

    handleTechTreeEscape();
}

// Tangani tombol ESC untuk kembali ke menu dari tech tree
void Game::handleTechTreeEscape() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        AudioManager::getInstance().stopMusic("bgm");
        state = GameState::MENU;
        m_transitionEffect.PlaySoundOut();
        m_transitionEffect.Start();
    }
}

void Game::DrawTechTree() {
    techTreeUI.Draw(m_currentPlayer);
}

// ===============================
// WORD DICTIONARY
// ===============================

void Game::UpdateDictionary() {
    AudioManager::getInstance().UpdateDefault();
    m_dictionary.Update();
    if (m_dictionary.WantsToGoBack()) {
        AudioManager::getInstance().stopMusic("bgm");
        state = GameState::MENU;
        m_transitionEffect.PlaySoundOut();
        m_transitionEffect.Start();
    }
}

void Game::DrawDictionary() {
    m_dictionary.Draw();
}

// ===============================
// UNLOCKED WORDS
// ===============================

void Game::UpdateUnlockedWords() {
    AudioManager::getInstance().UpdateDefault();
    m_unlockedWords.Update();
    if (m_unlockedWords.WantsToGoBack()) {
        AudioManager::getInstance().stopMusic("bgm");
        state = GameState::MENU;
        m_transitionEffect.PlaySoundOut();
        m_transitionEffect.Start();
    }
}

void Game::DrawUnlockedWords() {
    m_unlockedWords.Draw();
}

// ===============================
// SKIN SELECT
// ===============================

void Game::UpdateSkinSelect() {
    AudioManager::getInstance().UpdateDefault();
    bool backToMenu = false;
    bool goToGacha = false;
    skinSelect.Update(backToMenu, goToGacha);
    if (backToMenu) {
        AudioManager::getInstance().stopMusic("bgm");
        state = GameState::MENU;
        m_transitionEffect.PlaySoundOut();
        m_transitionEffect.Start();
    }
    if (goToGacha) {
        state = GameState::GACHA;
    }
}

void Game::DrawSkinSelect() {
    skinSelect.Draw();
}

// ===============================
// GACHA
// ===============================

void Game::UpdateGacha() {
    AudioManager::getInstance().UpdateDefault();
    bool backToSkinSelect = false;
    bool goToSkinSelect = false;
    gachaScreen.Update(backToSkinSelect, goToSkinSelect);
    if (backToSkinSelect || goToSkinSelect) {
        state = GameState::SKIN_SELECT;
    }
}

void Game::DrawGacha() {
    gachaScreen.Draw();
}


