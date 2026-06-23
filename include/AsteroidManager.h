#pragma once
#include "Asteroid.h"
#include "DifficultyManager.h"
#include "Timer.h"
#include "SinglyLinkedList.h"
#include <functional>
#include <queue>
#include <vector>

// Jenis event yang dimasukkan ke priority queue
enum EventType {
    NORMAL = 1,          // Spawn asteroid normal (prioritas lebih rendah)
    ASTEROID_SHOWER = 2  // Event hujan asteroid (prioritas lebih tinggi)
};

// Kelas pengelola seluruh asteroid: pool, shower event, dan priority queue eksekusi
class AsteroidManager
{
public:
    AsteroidManager();
    ~AsteroidManager();

    // Scan semua asteroid (pool + shower) dengan predicate, kumpulkan yang cocok
    std::vector<Asteroid*> scanAllAsteroids(std::function<bool(const Asteroid&)> predicate);
    // Scan dan ambil satu asteroid pertama yang cocok dengan predicate
    Asteroid *scanAsteroid(std::function<bool(const Asteroid&)> predicate);

    void update(float deltaTime);   // Update logika: timer, queue, pergerakan asteroid
    void draw();                    // Render semua asteroid aktif

    void setExplosionCallback(std::function<void(Vector2)> cb); // Set callback ledakan untuk semua asteroid

private:
    DifficultyManager difficultyManager; // Pengelola tingkat kesulitan berdasarkan waktu

    // -- ASTEROID POOL --
    // Array tetap 50 asteroid yang di-reuse (object pooling)
    Asteroid poolAsteroid[50];
    int asteroidCurrent = 0;            // Index untuk spawn asteroid berikutnya (round-robin)
    void spawnPoolAsteroid(int diff);   // Aktifkan asteroid dari pool dengan tier tertentu

    // -- EVENT ASTEROID SHOWER --
    // Linked list untuk node asteroid shower (tidak terbatas)
    SinglyLinkedList<Asteroid> asteroidShower;
    void addShowerNode();       // Tambah node baru ke linked list shower
    void triggerShowerWave();   // Mulai wave shower: reset cursor + start timer
    void updateShowerWave(float deltaTime); // Proses wave: aktifkan asteroid satu per satu per interval

    // -- TIMER --
    Timer timerNormal;          // Interval spawn asteroid normal
    Timer timerShower;          // Interval trigger event shower
    Timer timerShowerInterval;  // Interval antar asteroid dalam satu wave shower
    Timer timerExecute;         // Interval eksekusi event dari queue
    Timer timerAddNode;         // Interval penambahan node shower baru (60 detik)
    bool showerWaveActive = false; // Flag: apakah wave shower sedang berlangsung
    bool pendingShower = false;    // Flag: ada wave tertunda (saat freeze/lag)

    // -- PRIORITY QUEUE --
    // Event diproses berdasarkan prioritas: ASTEROID_SHOWER (2) > NORMAL (1)
    std::priority_queue<EventType> eventQueue;
    void executeEvent(); // Ambil dan eksekusi event dengan prioritas tertinggi

    // -- DEBUG --
    int getActiveAsteroidCount() const; // Hitung total asteroid aktif (pool + shower)

    std::function<void(Vector2)> m_explosionCallback; // Callback ledakan yang diteruskan ke Asteroid
};
