#pragma once
#include "raylib.h"
#include "State.h"
#include "Background.h"
#include "MainMenu.h"
#include <vector>
#include <string>

// testing
#include "GameplayManager.h"

class Game
{
public:
    Game();
    void Run();
private:
    bool statusMenuQuit;
    int score;
    GameState state;

    Background bg;
    MainMenu mainMenu;

    // testing
    GameplayManager gameplayManager;

    void Update();
    void Draw ();

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


