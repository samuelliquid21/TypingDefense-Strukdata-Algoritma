#include "Game.h"
#include "Leaderboard.h"
#include "Credit.h"
#include "AssetManager.h"
#include "SkinManager.h"
#include "raylib.h"
#include "rlgl.h"

void Game::Run() {
    while (!WindowShouldClose() && !statusMenuQuit) {
        Update();
        Draw();
    }
    openingScene.Unload();
    bg.Unload();
    LeaderboardSystem::Unload();
    UnloadMusicStream(musicLobby);
    UnloadMusicStream(musicCredit);
    UnloadSound(glitchMasuk);
    UnloadSound(glitchKeluar);
    if (sfxUniverseMoment.stream.buffer != nullptr) UnloadSound(sfxUniverseMoment);
    if (sfxCreditAmbient.stream.buffer != nullptr) UnloadSound(sfxCreditAmbient);
    if (sfxRadioNoise.stream.buffer != nullptr) UnloadSound(sfxRadioNoise);
    CloseAudioDevice();
    CloseWindow();
}

Game::Game() : gameplayManager(new GameplayManager()) {
    InitWindow(1080, 720, "Cosmic Keypad");
    InitAudioDevice();
    SetTargetFPS(60);
    SetExitKey(KEY_F12);

    // Set window icon
    Image icon = LoadImage("assets/img/icon.png");
    ImageResize(&icon, 64, 64);
    SetWindowIcon(icon);
    UnloadImage(icon);

    musicLobby  = LoadMusicStream("assets/sound/soundtrack.mp3");
    musicCredit = LoadMusicStream("assets/sound/cosmic.mp3");
    SetMusicVolume(musicLobby,  0.5f);
    SetMusicVolume(musicCredit, 0.5f);

    glitchMasuk  = LoadSound("assets/sound/glitchmasuk.mp3");
    glitchKeluar = LoadSound("assets/sound/glitchkeluar.mp3");

    // Load SFX from assets/sfx/
    sfxUniverseMoment = {0};
    universeMomentPlayed = false;
    if (FileExists("assets/sfx/universe_moment.mp3")) {
        sfxUniverseMoment = LoadSound("assets/sfx/universe_moment.mp3");
        SetSoundVolume(sfxUniverseMoment, 0.35f);
    }
    if (FileExists("assets/sfx/soul_serenity_sounds.mp3")) {
        sfxCreditAmbient = LoadSound("assets/sfx/soul_serenity_sounds.mp3");
        SetSoundVolume(sfxCreditAmbient, 0.12f);
    }
    if (FileExists("assets/sfx/noise_radio.mp3")) {
        sfxRadioNoise = LoadSound("assets/sfx/noise_radio.mp3");
        SetSoundVolume(sfxRadioNoise, 0.12f);
    }

    state          = GameState::LOADING;
    score          = 0;
    statusMenuQuit = false;
    isSaved        = false;
    highestScore   = 0;
    playerName     = "Ace";

    bg.Load("./assets/img/Space_Background.png", 20.0f);
    SkinManager::getInstance().init();
    gameplayManager->textureInit();
    LeaderboardSystem::Init();

    transitionTimer = 0.0f;
    isTransitioning = false;
    glitchIntensity = 0.0f;

    notifMeteorTimer = 45.0f;
    notifEnemyTimer  = 60.0f;
    notifNovaTimer   = 30.0f;
    enemyLevel       = 1;
    explosionDelayTimer = 0.0f;
    waitingForExplosion = false;
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
    notifMeteorTimer = 45.0f;
    notifEnemyTimer  = 60.0f;
    notifNovaTimer   = 30.0f;
    enemyLevel       = 1;
}

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

    if (state != GameState::PAUSE && state != GameState::LOADING && state != GameState::OPENING)
        bg.Update();

    switch (state) {
        case GameState::LOADING:     UpdateLoading();     break;
        case GameState::OPENING:     UpdateOpening();     break;
        case GameState::MENU:        UpdateMenu();        break;
        case GameState::GAMEPLAY:    UpdateGameplay();    break;
        case GameState::PAUSE:       UpdatePause();       break;
        case GameState::GAME_OVER:   UpdateGameOver();    break;
        case GameState::LEADERBOARD:  UpdateLeaderboard();  break;
        case GameState::CREDIT:       UpdateCredit();       break;
        case GameState::SKIN_SELECT:  UpdateSkinSelect();   break;
        case GameState::GACHA:        UpdateGacha();        break;
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
                Color gc = (i % 2 == 0)
                    ? (Color){ 0, 255, 200, (unsigned char)(intensity * 180) }
                    : (Color){ 255, 50, 120, (unsigned char)(intensity * 140) };
                DrawRectangle(0, y, 1080, h, gc);
            }
            if (GetRandomValue(0, 10) > 7)
                DrawRectangle(0, GetRandomValue(0,720), 1080, GetRandomValue(1,3), {255,255,255,100});
        }
    } else {
        if (state != GameState::LOADING) bg.Draw();
        switch (state) {
            case GameState::LOADING:     DrawLoading();     break;
            case GameState::OPENING:     DrawOpening();     break;
            case GameState::MENU:        DrawMenu();        break;
            case GameState::GAMEPLAY:    DrawGameplay();    break;
            case GameState::PAUSE:       DrawPause();       break;
            case GameState::GAME_OVER:   DrawGameOver();    break;
            case GameState::LEADERBOARD: DrawLeaderboard(); break;
            case GameState::CREDIT:      DrawCredit();      break;
            case GameState::SKIN_SELECT: DrawSkinSelect();  break;
            case GameState::GACHA:       DrawGacha();       break;
            default: break;
        }
    }
    EndDrawing();
}

void Game::UpdateLoading() {
    bool done = false;
    loadingScreen.Update(done);
    if (done) {
        openingScene.Init();
        state = GameState::OPENING;
    }
}
void Game::DrawLoading() { loadingScreen.Draw(); }

void Game::UpdateOpening() {
    bool done = false;
    openingScene.Update(done);
    if (done) {
        // Play universe moment as dramatic transition
        if (sfxUniverseMoment.stream.buffer != nullptr && !universeMomentPlayed) {
            PlaySound(sfxUniverseMoment);
            universeMomentPlayed = true;
        }
        openingScene.Unload();
        PlayMusicStream(musicLobby);
        SeekMusicStream(musicLobby, 5.0f);
        state = GameState::MENU;
    }
}
void Game::DrawOpening() { openingScene.Draw(); }

void Game::UpdateMenu() {
    UpdateMusicStream(musicLobby);
    if (!IsMusicStreamPlaying(musicLobby)) {
        PlayMusicStream(musicLobby);
        SeekMusicStream(musicLobby, 5.0f);
    }

    mainMenu.Update();

    if (mainMenu.IsOptionChosen()) {
        int choice = mainMenu.GetSelectedIndex();
        if (choice == 0) {
            StopMusicStream(musicLobby);
            gameOver.Reset();
            restartGame();
            eventNotif.TriggerFindNova();
            gameplayManager->setSkinFromManager();
            state = GameState::GAMEPLAY;
        } else if (choice == 1) {
            StopMusicStream(musicLobby);
            LeaderboardSystem::Init();
            state = GameState::LEADERBOARD;
        } else if (choice == 2) {
            state = GameState::SKIN_SELECT;
        } else if (choice == 3) {
            StopMusicStream(musicLobby);
            if (!IsSoundPlaying(glitchMasuk)) PlaySound(glitchMasuk);
            isTransitioning = true;
            transitionTimer = 0.6f;
            targetState = GameState::CREDIT;
        } else if (choice == 4) {
            statusMenuQuit = true;
        }
        mainMenu.ResetChoice();
    }
}
void Game::DrawMenu() { mainMenu.Draw(); }

void Game::UpdateGameplay() {
    float dt = GetFrameTime();

    // Handle explosion delay
    if (waitingForExplosion) {
        gameplayManager->update(dt);
        eventNotif.Update(dt);
        explosionDelayTimer -= dt;
        if (explosionDelayTimer <= 0.0f || gameplayManager->isShipDestroyed()) {
            waitingForExplosion = false;
            score = gameplayManager->score;
            gameOver.SetScore(score);
            restartGame();
            state = GameState::GAME_OVER;
        }
        return;
    }

    gameplayManager->update(dt);
    eventNotif.Update(dt);

    // Timer notifikasi meteor shower
    notifMeteorTimer -= dt;
    if (notifMeteorTimer <= 0.0f) {
        eventNotif.TriggerMeteorShower();
        notifMeteorTimer = 45.0f + (float)GetRandomValue(-5, 10);
    }

    // Timer notifikasi enemy upgrade
    notifEnemyTimer -= dt;
    if (notifEnemyTimer <= 0.0f) {
        enemyLevel++;
        eventNotif.TriggerEnemyUpgrade(enemyLevel);
        notifEnemyTimer = 60.0f;
    }

    // Timer reminder Nova
    notifNovaTimer -= dt;
    if (notifNovaTimer <= 0.0f) {
        int r = GetRandomValue(0, 3);
        if (r == 0)
            eventNotif.TriggerBossWarning("Ace, no sign of Nova yet. Stay sharp out there.");
        else if (r == 1)
            eventNotif.TriggerFindNova();
        else if (r == 2)
            eventNotif.TriggerBossWarning("I'm picking up an anomaly near your position. Could be Nova's trail.");
        else
            eventNotif.TriggerBossWarning("Whatever Nova found out there — don't stop. We need to know.");
        notifNovaTimer = 80.0f + (float)GetRandomValue(-10, 20);
    }

    if (gameplayManager->isHit()) {
        score = gameplayManager->score;
        gameOver.SetScore(score);
        waitingForExplosion = true;
        explosionDelayTimer = 1.5f;
        SkinManager::getInstance().addRP(gameplayManager->getSessionRP());
        SkinManager::getInstance().save();
    }
    if (IsKeyPressed(KEY_ESCAPE) && !gameplayManager->isExplodingNow()) state = GameState::PAUSE;
}
void Game::DrawGameplay() {
    gameplayManager->draw();
    eventNotif.Draw();
}

void Game::UpdatePause() {
    pauseMenu.Update();
    if (pauseMenu.IsOptionChosen()) {
        if (pauseMenu.GetSelectedIndex() == 0)
            pauseMenu.StartCountdown();
        else if (pauseMenu.GetSelectedIndex() == 1) {
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
void Game::DrawPause() { gameplayManager->draw(); pauseMenu.Draw(); }

void Game::UpdateGameOver() {
    gameOver.Update();
    if (gameOver.ShouldReturnToMenu()) {
        universeMomentPlayed = false;
        PlayMusicStream(musicLobby);
        SeekMusicStream(musicLobby, 5.0f);
        state = GameState::MENU;
    }
}
void Game::DrawGameOver() { gameOver.Draw(); }

void Game::UpdateLeaderboard() {
    bool kembali = false;
    LeaderboardSystem::Update(kembali);
    if (kembali) {
        PlayMusicStream(musicLobby);
        SeekMusicStream(musicLobby, 5.0f);
        state = GameState::MENU;
    }
}
void Game::DrawLeaderboard() { LeaderboardSystem::Draw(); }

void Game::UpdateCredit() {
    UpdateMusicStream(musicCredit);
    if (!IsMusicStreamPlaying(musicCredit)) {
        PlayMusicStream(musicCredit);
        SeekMusicStream(musicCredit, 5.0f);
    }

    // Play soul_serenity ambient
    if (sfxCreditAmbient.stream.buffer != nullptr && !IsSoundPlaying(sfxCreditAmbient)) {
        PlaySound(sfxCreditAmbient);
    }

    bool backToMenu = false;
    creditScreen.Update(backToMenu);
    if (backToMenu && !isTransitioning) {
        StopMusicStream(musicCredit);
        if (sfxCreditAmbient.stream.buffer != nullptr) StopSound(sfxCreditAmbient);
        if (!IsSoundPlaying(glitchKeluar)) PlaySound(glitchKeluar);
        isTransitioning = true;
        transitionTimer = 0.6f;
        targetState = GameState::MENU;
    }
}
void Game::DrawCredit() { creditScreen.Draw(); }

void Game::UpdateSkinSelect() {
    bool backToMenu = false;
    bool goToGacha = false;
    skinSelect.Update(backToMenu, goToGacha);
    if (backToMenu) {
        state = GameState::MENU;
    } else if (goToGacha) {
        state = GameState::GACHA;
    }
}

void Game::DrawSkinSelect() { skinSelect.Draw(); }

void Game::UpdateGacha() {
    bool backToSkinSelect = false;
    bool goToSkinSelect = false;
    gachaScreen.Update(backToSkinSelect, goToSkinSelect);
    if (backToSkinSelect) {
        state = GameState::SKIN_SELECT;
    } else if (goToSkinSelect) {
        state = GameState::SKIN_SELECT;
    }
}

void Game::DrawGacha() { gachaScreen.Draw(); }
