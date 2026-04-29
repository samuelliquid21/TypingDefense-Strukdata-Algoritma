#pragma once
#include "raylib.h"
#include "State.h"
#include "Background.h"
#include "MainMenu.h"
#include <vector>
#include <string>

// testing
#include "GameplayManager.h"
#include "Pause.h"

// Kelas utama game TypingDefense
// Mengontrol semua state game dan game loop
class Game
{
public:
    Game();
    ~Game();
    void Run();  // Main game loop
private:
    bool statusMenuQuit;  // Flag untuk quit dari menu
    int score;            // Score player
    GameState state;      // State game saat ini (menu, gameplay, pause, dll)

    Background bg;        // Background scrolling
    MainMenu mainMenu;    // Menu utama

    GameplayManager* gameplayManager;  // Pointer ke manajer gameplay (dinamik)
    PauseMenu pauseMenu;  // Menu pause

    void Update();  // Update utama (memanggil Update berdasarkan state)
    void Draw ();   // Draw utama (memanggil Draw berdasarkan state)
    void restartGame();  // Reset game ke kondisi awal

    // Fungsi update untuk setiap state
    void UpdateMenu();       // Update state menu
    void UpdateGameplay();   // Update state gameplay
    void UpdatePause();      // Update state pause
    void UpdateGameOver();   // Update state game over
    void UpdateLeaderboard();// Update leaderboard
    void UpdateCredit();     // Update credit screen

    // Fungsi draw untuk setiap state
    void DrawMenu();        // Draw menu
    void DrawGameplay();    // Draw gameplay
    void DrawPause();       // Draw pause
    void DrawGameOver();    // Draw game over
    void DrawLeaderboard(); // Draw leaderboard
    void DrawCredit();      // Draw credit
};


