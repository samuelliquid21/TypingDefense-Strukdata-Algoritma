#include "SpaceShip.h"
#include "AsteroidManager.h"
#include "Combostack.h"
#include "raylib.h"

// State untuk typing game: searching atau locked pada target
enum typingState {
    SEARCH_FOR_TARGET,  // Sedang mencari asteroid dengan karakter pertama
    TARGET_LOCKED       // Sudah lock target, mengetik sisa kata
};

// Type alias untuk function pointer callbacks
using ScoreCallback = void(*)(int score, int multiplier);  // Callback saat score berubah
using AsteroidDestroyedCallback = void(*)(const char* word);  // Callback saat asteroid hancur

// Kelas utama untuk manajemen gameplay (score, combo, asteroid, spaceship)
// Menggunakan callback functions dan function overloading
class GameplayManager
{
private:
    SpaceShip spaceship;           // Player spaceship
    AsteroidManager asteroidManager;  // Manajer asteroid
    ComboStack comboStack;         // Stack untuk combo multiplier
    typingState state = typingState::SEARCH_FOR_TARGET;  // State typing saat ini
    Asteroid* currentTarget = nullptr;  // Pointer ke asteroid yang sedang ditarget
    int wordsCompleted = 0;        // Counter kata yang telah diselesaikan
    bool wasPreviousKeyWrong = false;  // Flag untuk key salah sebelumnya

    ScoreCallback onScoreChanged = nullptr;  // Callback pointer (nullable)
    AsteroidDestroyedCallback onAsteroidDestroyed = nullptr;  // Callback pointer (nullable)

public:
    int score = 0;  // Score player

    // Function overloading: AddScore dengan 1 atau 2 parameter
    void AddScore(int points);  // Gunakan current combo multiplier
    void AddScore(int basePoints, int multiplier);  // Gunakan multiplier custom

    // Setter untuk callback functions
    void SetScoreCallback(ScoreCallback callback);
    void SetAsteroidDestroyedCallback(AsteroidDestroyedCallback callback);

    bool isHit();  // Cek collision dengan player

    void update(float deltaTime);  // Update game logic
    void draw();    // Render game objects
    void reset();   // Reset ke kondisi awal
};