#pragma once
#include "raylib.h"
#include <vector>
#include <string>

// Class untuk menu pause dengan countdown timer
// Menyediakan opsi Resume dan Back to Menu
// Fitur countdown: setelah memilih Resume, timer 3-2-1-GO sebelum game dilanjutkan
class PauseMenu {
public:
    PauseMenu();

    void Update();  // Handle input saat pause, atau update countdown timer
    void Draw();    // Render menu pause (overlay hitam) atau tampilan countdown

    // INLINE FUNCTIONS untuk getter sederhana (performansi lebih baik)
    inline int GetSelectedIndex() const { return selectedIndex; }
    inline bool IsOptionChosen() const { return optionChosen; }
    
    // Cek state countdown
    inline bool IsCountingDown() const { return isCountingDown; }
    inline bool IsCountdownFinished() const { return countdownTimer <= -0.5f; }
    inline void StartCountdown() { isCountingDown = true; countdownTimer = 3.99f; optionChosen = false; }
    
    inline void Reset() { optionChosen = false; isCountingDown = false; countdownTimer = 0.0f; selectedIndex = 0; }

private:
    std::string title;                       // Judul "PAUSED"
    std::vector<std::string> options;        // Daftar opsi: Resume, Back to Menu

    int selectedIndex;                       // Index opsi yang terpilih
    bool optionChosen;                       // Flag: user sudah memilih opsi

    int titleFontSize;
    int menuFontSize;

    bool isCountingDown;                     // Apakah sedang dalam state countdown?
    float countdownTimer;                    // Timer countdown (3.0 -> 2.0 -> 1.0 -> 0 -> "GO!")

    void handleNavigation();                 // Navigasi opsi naik/turun dengan wrap-around
    void drawCountdown();                    // Tampilkan angka countdown (3, 2, 1, GO!)
    void drawMenuOptions();                  // Tampilan menu pause dengan pilihan
};
