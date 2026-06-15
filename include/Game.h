#pragma once
#include "raylib.h"
#include "State.h"
#include "Background.h"
#include "MainMenu.h"
#include "GameOver.h"
#include "Credit.h"
#include "ScoreManager.h"
#include "GameplayManager.h"
#include "Pause.h"
#include "LoadingScreen.h"
#include "OpeningScene.h"
#include "EventNotification.h"
#include "SkinSelect.h"
#include "GachaScreen.h"
#include <vector>
#include <string>

class Game {
public:
    Game();
    ~Game();
    void Run();

private:
    bool statusMenuQuit;
    int score;
    GameState state;

    Background bg;
    MainMenu mainMenu;
    GameOver gameOver;
    PauseMenu pauseMenu;
    Credit creditScreen;

    // === NEW: Loading & Opening ===
    LoadingScreen  loadingScreen;
    OpeningScene   openingScene;
    EventNotification eventNotif;

    GameplayManager* gameplayManager;
    SkinSelect skinSelect;
    GachaScreen gachaScreen;
    bool isSaved;
    int highestScore;
    std::string playerName;

    // AUDIO
    Music musicCredit;
    Music musicLobby;

    // Transisi & Glitch
    float transitionTimer;
    bool  isTransitioning;
    float glitchIntensity;
    GameState targetState;

    Sound glitchMasuk;
    Sound glitchKeluar;

    // SFX from assets/sfx/
    Sound sfxUniverseMoment;
    Sound sfxCreditAmbient;
    Sound sfxRadioNoise;
    bool universeMomentPlayed;

    // === Notif event timers ===
    float notifMeteorTimer;
    float notifEnemyTimer;
    float notifNovaTimer;
    int   enemyLevel;

    // Explosion delay
    float explosionDelayTimer;
    bool  waitingForExplosion;

    void Update();
    void Draw();
    void restartGame();

    void UpdateLoading();
    void UpdateOpening();
    void UpdateMenu();
    void UpdateGameplay();
    void UpdatePause();
    void UpdateGameOver();
    void UpdateLeaderboard();
    void UpdateCredit();
    void UpdateSkinSelect();
    void UpdateGacha();

    void DrawLoading();
    void DrawOpening();
    void DrawMenu();
    void DrawGameplay();
    void DrawPause();
    void DrawGameOver();
    void DrawLeaderboard();
    void DrawCredit();
    void DrawSkinSelect();
    void DrawGacha();
};
