#pragma once
#include "raylib.h"
#include "State.h"
#include "Background.h"
#include "MainMenu.h"
#include "GameOver.h"
#include "Credit.h"
#include "GameplayManager.h"
#include "Pause.h"
#include "DataManager.h"
#include "TechTree.h"
#include "TechTreeUI.h"
#include "Dictionary.h"
#include "UnlockedWords.h"
#include <vector>
#include <string>
#include "LoginScreen.h"
#include "RegisterScreen.h"
#include "LogoutScreen.h"
#include "TransitionEffect.h"
#include "DebugOverlay.h"
#include "SkinSelect.h"
#include "GachaScreen.h"
#include "OpeningScene.h"
#include "LoadingScreen.h"
#include "EventNotification.h"

// Kelas utama game — mengelola state machine, game loop, dan seluruh subsistem.
// Menggunakan pola Game::Run() -> Update() / Draw() yang dipanggil tiap frame.
class Game {
public:
    Game();
    ~Game();
    void Run();         // Loop utama hingga window ditutup atau player keluar

private:
    bool statusMenuQuit;
    int score;
    GameState state;

    // === Subsistem Tampilan ===
    Background bg;        // Background scrolling
    MainMenu mainMenu;    // Menu utama
    GameOver gameOver;    // Game over screen
    PauseMenu pauseMenu;
    Credit creditScreen;

    // === Subsistem Gameplay ===
    GameplayManager* gameplayManager;

    // === Subsistem Akun ===
    LoginScreen loginScreen;
    RegisterScreen registerScreen;
    LogoutScreen logoutScreen;

    // === Tech Tree & Dictionary ===
    TechTree techTree;
    TechTreeUI techTreeUI;
    Dictionary m_dictionary;
    UnlockedWords m_unlockedWords;

    // === Data Pemain ===
    PlayerProfile m_currentPlayer;
    PlayerProfile m_sessionBackup;
    bool m_isLoggedIn = false;

    // === Transisi & Efek Glitch ===
    TransitionEffect m_transitionEffect;

    // === Opening & Loading ===
    OpeningScene openingScene;
    LoadingScreen loadingScreen;
    EventNotification eventNotif;

    // === Metode Update per State ===
    void Update();
    void Draw();
    void restartGame();
    void setupCallbacks();

    void UpdateLoading();
    void UpdateOpening();
    void UpdateMenu();
    void UpdateGameplay();
    void UpdatePause();
    void UpdateGameOver();
    void UpdateLeaderboard();
    void UpdateCredit();
    void UpdateLoginRegister();
    void UpdateRegister();
    void UpdateLogout();
    void UpdateTechTree();
    void UpdateDictionary();
    void UpdateUnlockedWords();
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
    void DrawLoginRegister();
    void DrawRegister();
    void DrawLogout();
    void DrawTechTree();
    void DrawDictionary();
    void DrawUnlockedWords();
    void DrawSkinSelect();
    void DrawGacha();

    // === Extracted Helper Methods ===
    void initAudioAndWindow();
    void initGameStateAndAssets();
    void dispatchUpdateState();
    void dispatchDrawState();
    void handleMenuF1Key();
    void handleMenuChoice();
    void startGameplay();
    void openLeaderboardFromMenu();
    void openSkinSelectFromMenu();
    void openTechTreeFromMenu();
    void openUnlockedWordsFromMenu();
    void openCreditFromMenu();
    void openLogoutFromMenu();
    void quitGame();
    void handleGameplayHit();
    void handlePauseChoice();
    void handleTechTreeEscape();

    SkinSelect skinSelect;
    GachaScreen gachaScreen;

};
