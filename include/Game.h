#pragma once
#include "raylib.h"
#include "State.h"
#include "Background.h"
#include "MainMenu.h"
#include <vector>
#include <string>

// testing
#include "AsteroidManager.h"

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
    AsteroidManager asteroidManager;

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


