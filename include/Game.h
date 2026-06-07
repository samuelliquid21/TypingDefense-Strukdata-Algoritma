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

    Background bg;        // Background scrolling
    MainMenu mainMenu;    // Menu utama
    GameOver gameOver;    // Game over screen
    PauseMenu pauseMenu;
    Credit creditScreen;

    GameplayManager* gameplayManager;

    TechTree techTree;
    TechTreeUI techTreeUI;
    Dictionary m_dictionary;

    PlayerProfile m_currentPlayer;
    bool m_isLoggedIn = false;

    // AUDIO - DAFTARKAN SEMUA DI SINI
    Music musicCredit; 
    Music musicLobby;  

    // Variabel Transisi & Glitch (LOGIKA DARI LEADERBOARD)
    float transitionTimer;
    bool isTransitioning;
    float glitchIntensity; 
    GameState targetState;

    Sound glitchMasuk;
    Sound glitchKeluar;

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

    void DrawPlayerInfo();
};
