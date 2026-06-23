#pragma once

#include "SpaceShip.h"
#include "AsteroidManager.h"
#include "ComboStack.h"
#include "ShieldSkill.h"
#include "BombSkill.h"
#include "ScoreBoosterSkill.h"
#include "AuraFieldSkill.h"
#include "InstantCritSkill.h"
#include "ExplosionManager.h"
#include <functional>

// State mesin untuk sistem typing: mencari target vs sedang mengetik target
enum typingState {
    SEARCH_FOR_TARGET,   // Mencari asteroid baru untuk ditarget
    TARGET_LOCKED        // Sedang mengetik kata pada asteroid yang sudah ditarget
};

// Type alias untuk callback function pointer dan std::function
using ScoreCallback = void(*)(int score, int multiplier);              // Dipanggil saat skor berubah
using AsteroidDestroyedCallback = std::function<void(const std::string& word)>;  // Dipanggil saat asteroid hancur

// Kelas utama pengatur seluruh gameplay: skor, combo, asteroid, spaceship, skill, dan state typing
class GameplayManager
{
private:
    // Binding skill aktif: pointer ke skill + nama key JSON + nomor tombol (1..N)
    struct SkillBinding {
        Skill* skill;
        std::string keyName;
        int keyNumber;
    };

    SpaceShip spaceship;                                    // Objek spaceship pemain
    AsteroidManager asteroidManager;                        // Pengelola seluruh asteroid (pool, shower, event queue)
    ComboStack comboStack;                                  // Stack untuk combo multiplier (max 6 level)
    ShieldSkill shieldSkill;                                 // Skill perisai pelindung pemain
    BombSkill bombSkill;                                     // Skill bom shockwave area
    ScoreBoosterSkill scoreBoosterSkill;                       // Skill multiplier 16x skor
    AuraFieldSkill auraFieldSkill;                             // Skill shield durasi 10 detik
    InstantCritSkill instantCritSkill;                         // Skill instant crit 10 detik
    ExplosionManager explosionManager;                       // Efek partikel ledakan
    typingState state = typingState::SEARCH_FOR_TARGET;     // State typing saat ini
    Asteroid* currentTarget = nullptr;                      // Pointer ke asteroid yang sedang diketik
    bool wasPreviousKeyWrong = false;                       // Flag untuk mencegah error sound berulang

    ScoreCallback onScoreChanged = nullptr;                     // Callback saat skor berubah (nullable)
    AsteroidDestroyedCallback onAsteroidDestroyed = nullptr;    // Callback saat asteroid hancur (nullable)

    const std::vector<std::string>* m_unlockedSkills = nullptr; // Pointer ke daftar skill yang sudah di-unlock
    std::vector<SkillBinding> m_activeSkills;                    // Skill aktif yg ter-unlock (dibangun ulang otomatis)

    // === Extracted Helper Methods ===
    int getHitRadius() const;
    bool handleAuraAbsorption(const std::vector<Asteroid*>& hits);
    bool handleShieldAbsorption(const std::vector<Asteroid*>& hits);
    void triggerGameOver();
    void updateSubsystems(float deltaTime);
    void updateActiveSkills(float deltaTime);
    void handleBombShockwave();
    void processAllCharInputs();
    bool isTargetInvalid() const;
    void resetToSearchTarget();
    void processSearchForTarget(char c);
    Asteroid* findNearestTarget(char c);
    void playSearchError();
    void handleInstantCritOrLock();
    void processTargetTyping(char c);
    void handleSuccessfulTyping(int result);
    void drawGameObjects();
    void drawScoreAndCombo();
    void drawActiveSkillsHUD();
    void drawSkillBindingLine(const SkillBinding& binding, int& hudY);

public:
    int   score = 0;   // Skor pemain saat ini
    int   totalKeystrokes   = 0;
    int   correctKeystrokes = 0;
    float survivalTime      = 0.0f;
    int wordsCompleted = 0;

    GameplayManager() = default; // Constructor default
    ~GameplayManager();          // Audio lifecycle via AudioManager

    void AddScore(int points);                      // Tambah skor dengan multiplier combo stack
    void AddScore(int basePoints, int multiplier);  // Tambah skor dengan multiplier kustom + callback

    void SetScoreCallback(ScoreCallback callback);                 // Daftarkan callback perubahan skor
    void SetAsteroidDestroyedCallback(AsteroidDestroyedCallback callback); // Daftarkan callback asteroid hancur

    void textureInit(); // Inisialisasi texture spaceship dan load sound effect

    void setUnlockedSkills(const std::vector<std::string>* skills);
    bool isUnlocked(const std::string& name) const;
    void rebuildActiveSkills();

    bool isHit(); // Cek tabrakan asteroid dengan player (shield atau tidak)

    void update(float deltaTime);   // Update utama: state machine typing, skill, input
    void draw();                    // Render semua objek game + HUD
};
