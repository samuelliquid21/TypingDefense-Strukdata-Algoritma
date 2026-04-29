#pragma once
#include "raylib.h"
#include <vector>
#include <string>

// Class untuk menu pause dengan countdown timer
// Menggunakan INLINE FUNCTIONS untuk simple getters/setters
class PauseMenu {
public:
    PauseMenu();

    void Update();  // Handle input saat pause
    void Draw();    // Render menu pause

    // INLINE FUNCTIONS (didefinisikan di header untuk performance)
    inline int GetSelectedIndex() const { return selectedIndex; }
    inline bool IsOptionChosen() const { return optionChosen; }
    
    inline bool IsCountingDown() const { return isCountingDown; }
    inline bool IsCountdownFinished() const { return countdownTimer <= -0.5f; }
    inline void StartCountdown() { isCountingDown = true; countdownTimer = 3.99f; optionChosen = false; }
    
    inline void Reset() { optionChosen = false; isCountingDown = false; countdownTimer = 0.0f; selectedIndex = 0; }

private:
    std::string title;
    std::vector<std::string> options;

    int selectedIndex;     // Index option yang terpilih
    bool optionChosen;     // Flag apakah option sudah dipilih

    int titleFontSize;
    int menuFontSize;

    bool isCountingDown;   // Flag apakah sedang countdown
    float countdownTimer;  // Timer untuk countdown (3.99 -> 0)
};