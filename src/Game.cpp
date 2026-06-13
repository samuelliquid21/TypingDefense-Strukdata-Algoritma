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
    // Loop utama: berjalan selama window tidak ditutup dan player belum pilih quit
    while (!WindowShouldClose() && !statusMenuQuit) {
        Update();
        Draw();
    }

    // Bersihkan resource sebelum keluar
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
    SetExitKey(KEY_F12);         // F12 untuk force close (debug)

    // Setup musik lobby (diputar di menu)
    musicLobby = LoadMusicStream("assets/sound/soundtrack.mp3");
    SetMusicVolume(musicLobby, 0.5f);
    PlayMusicStream(musicLobby);
    SeekMusicStream(musicLobby, 5.0f); // Skip 5 detik awal biar langsung rame

    state = GameState::MENU;
    score = 0;
    statusMenuQuit = false;

    DataManager::getInstance().load();

    techTree.loadFromProfile(m_currentPlayer);

    bg.Load("./assets/img/Space_Background.png", 20.0f);
    gameplayManager->textureInit();

    // Callback saat asteroid hancur: unlock word dan beri research point
    gameplayManager->SetAsteroidDestroyedCallback(
        [this](const std::string& word) {
            // Hindari duplikasi kata
            for (const auto& w : m_currentPlayer.unlocked_words)
                if (w == word) return;
            m_currentPlayer.unlocked_words.push_back(word);
            m_currentPlayer.research_point += 10;
            DataManager::getInstance().SavePlayer(m_currentPlayer);
        }
    );

    LeaderboardSystem::Init();

    // Setup musik credit
    musicCredit = LoadMusicStream("assets/sound/cosmic.mp3");
    SetMusicVolume(musicCredit, 0.5f);

    glitchMasuk = LoadSound("assets/sound/glitchmasuk.mp3");
    glitchKeluar = LoadSound("assets/sound/glitchkeluar.mp3");

    // Inisialisasi variabel transisi
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
    // Buang manager lama dan buat baru untuk mereset seluruh state gameplay
    delete gameplayManager;
    gameplayManager = new GameplayManager();
    gameplayManager->textureInit();

    // Pasang ulang callback asteroid destroy
    gameplayManager->SetAsteroidDestroyedCallback(
        [this](const std::string& word) {
            for (const auto& w : m_currentPlayer.unlocked_words)
                if (w == word) return;
            m_currentPlayer.unlocked_words.push_back(word);
            m_currentPlayer.research_point += 10;
            DataManager::getInstance().SavePlayer(m_currentPlayer);
        }
    );
}

// ===============================
// 🔄 CORE LOOP SYSTEM
// ===============================

void Game::Update() {
    float dt = GetFrameTime();

    // Jika sedang dalam transisi glitch, hitung mundur timer
    if (isTransitioning) {
        transitionTimer -= dt;
        glitchIntensity = transitionTimer / 0.6f; // Normalisasi intensitas

        if (transitionTimer <= 0) {
            state = targetState; // Pindah ke state tujuan
            isTransitioning = false;
            glitchIntensity = 0.0f;
        }
        return; // Skip update state lain selama transisi
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

    if (isTransitioning) {
        // Selama transisi, gambar efek glitch di atas background
        ClearBackground(BLACK);
        float intensity = transitionTimer / 0.6f;
        bg.Draw();

        if (intensity > 0.01f) {
            // Gambar garis-garis glitch acak cyan/pink
            for (int i = 0; i < 12; i++) {
                int y = GetRandomValue(0, 720);
                int h = GetRandomValue(5, 25);

                // Cyan & Pink/Magenta — warna identik efek dari Leaderboard
                Color glitchColor = (i % 2 == 0) ?
                    (Color){ 0, 255, 200, (unsigned char)(intensity * 180) } :
                    (Color){ 255, 50, 120, (unsigned char)(intensity * 140) };

                DrawRectangle(0, y, 1080, h, glitchColor);
            }

            // Kadang-kadang tambah garis putih tipis untuk variasi
            if (GetRandomValue(0, 10) > 7) {
                DrawRectangle(0, GetRandomValue(0, 720), 1080, GetRandomValue(1, 3), {255, 255, 255, 100});
            }
        }

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

    UpdateMusicStream(musicLobby);

    // Pastikan musik lobby selalu terputar; jika berhenti (looping), mainkan ulang
    if (!IsMusicStreamPlaying(musicLobby)) {
        PlayMusicStream(musicLobby);
        SeekMusicStream(musicLobby, 5.0f);
    }

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
            // Mulai game: hentikan musik lobby, reset game over, restart gameplay
            StopMusicStream(musicLobby);
            gameOver.Reset();
            restartGame();
            state = GameState::GAMEPLAY;
        }
        else if (choice == 1) {
            // Buka leaderboard
            StopMusicStream(musicLobby);
            LeaderboardSystem::Init();
            state = GameState::LEADERBOARD;
        }
        else if (choice == 2) {
            // Buka tech tree / unlock skill
            state = GameState::UNLOCK_SKILL;
        }
        else if (choice == 3) {
            // Lihat kata yang sudah di-unlock
            m_unlockedWords.BuildFromPlayer(m_currentPlayer);
            state = GameState::UNLOCKED_WORDS;
        }
        else if (choice == 4) {
            // Transisi ke credit dengan efek glitch
            StopMusicStream(musicLobby);
            if (!IsSoundPlaying(glitchMasuk)) PlaySound(glitchMasuk);
            isTransitioning = true;
            transitionTimer = 0.6f;
            targetState = GameState::CREDIT;
        }
        else if (choice == 5) {
            // Logout dan kembali ke login screen
            logoutScreen.Reset();
            state = GameState::LOGOUT;
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

    // Tampilkan info player (debug) jika diaktifkan
    if (m_isLoggedIn && Config::enableDebugPlayerInfo) {
        DrawPlayerInfo();
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
            if (score > m_currentPlayer.highest_score)
                m_currentPlayer.highest_score = score;
                m_currentPlayer.accuracy         = gameplayManager->GetAccuracy();
                m_currentPlayer.enemies_defeated = gameplayManager->enemiesDefeated;
                m_currentPlayer.survival_time    = gameplayManager->survivalTime;
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
            // Kembali ke menu — reset gameplay
            pauseMenu.Reset();
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
    if (kembali) state = GameState::MENU;
}

void Game::DrawLeaderboard() {
    LeaderboardSystem::Draw();
}

// ===============================
// CREDIT STATE
// ===============================

void Game::UpdateCredit() {
    UpdateMusicStream(musicCredit);
    if (!IsMusicStreamPlaying(musicCredit)) {
        PlayMusicStream(musicCredit);
        SeekMusicStream(musicCredit, 5.0f); // Langsung skip intro credit 5 detik
    }

    bool backToMenu = false;
    creditScreen.Update(backToMenu);

    // Transisi kembali ke menu dengan efek glitch
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

// ===============================
// LOGIN / LOGOUT (BELUM IMPLEMENTASI)
// ===============================

void Game::UpdateLoginRegister() {
    UpdateMusicStream(musicLobby);
    if (!IsMusicStreamPlaying(musicLobby)) {
        PlayMusicStream(musicLobby);
    }

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
    UpdateMusicStream(musicLobby);
    if (!IsMusicStreamPlaying(musicLobby)) {
        PlayMusicStream(musicLobby);
    }

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
        state = GameState::MENU;
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
        state = GameState::MENU;
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
        state = GameState::MENU;
    }
}

void Game::DrawUnlockedWords() {
    m_unlockedWords.Draw();
}

// ===============================
// PLAYER INFO PANEL (DEBUG)
// ===============================

void Game::DrawPlayerInfo() {
    int panelWidth = 320;
    int panelHeight = 150;
    int padding = 15;
    int x = Config::screenWidth - panelWidth - 20;
    int y = Config::screenHeight - panelHeight - 20;

    // Background panel semi-transparan dengan border cyan
    DrawRectangle(x - 10, y - 10, panelWidth + 20, panelHeight + 20, Color{0, 0, 0, 180});
    DrawRectangleLines(x - 10, y - 10, panelWidth + 20, panelHeight + 20, Color{0, 255, 200, 200});

    int fontSize = 16;
    int lineHeight = 24;
    int curY = y + 5;

    // Tampilkan data profil pemain
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
