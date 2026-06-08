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

    // === Tech Tree & Dictionary ===
    TechTree techTree;
    TechTreeUI techTreeUI;
    Dictionary m_dictionary;
    UnlockedWords m_unlockedWords;

    // === Data Pemain ===
    PlayerProfile m_currentPlayer;
    bool m_isLoggedIn = false;

    // === Audio ===
    Music musicCredit;
    Music musicLobby;

    // === Transisi & Efek Glitch (dari Leaderboard) ===
    float transitionTimer;
    bool isTransitioning;
    float glitchIntensity;
    GameState targetState;

    Sound glitchMasuk;
    Sound glitchKeluar;

    // === Metode Update per State ===
    void Update();
    void Draw();
    void restartGame();

    void UpdateMenu();
    void UpdateGameplay();
    void UpdatePause();
    void UpdateGameOver();
    void UpdateLeaderboard();
    void UpdateCredit();
    void UpdateLoginRegister();
    void UpdateLogout();
    void UpdateTechTree();
    void UpdateDictionary();
    void UpdateUnlockedWords();

    void DrawMenu();
    void DrawGameplay();
    void DrawPause();
    void DrawGameOver();
    void DrawLeaderboard();
    void DrawCredit();
    void DrawLoginRegister();
    void DrawLogout();
    void DrawTechTree();
    void DrawDictionary();
    void DrawUnlockedWords();

    void DrawPlayerInfo();
};
