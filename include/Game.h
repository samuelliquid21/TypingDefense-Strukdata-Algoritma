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
    bool isSaved;
    int highestScore;
    std::string playerName;

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

    void DrawMenu();
    void DrawGameplay();
    void DrawPause();
    void DrawGameOver();
    void DrawLeaderboard();
    void DrawCredit();
};