#pragma once
#include "Asteroid.h"
#include "DifficultyManager.h"
#include "Timer.h"
#include "SinglyLinkedList.h"
#include <functional>
#include <queue>
#include <vector>

enum EventType {
    NORMAL = 1,
    ASTEROID_SHOWER = 2
};

class AsteroidManager2
{
public:

    // ==== CONSTRUCTOR DAN DESTRUCTOR ====

    AsteroidManager2();
    ~AsteroidManager2();

    // ==== FUNGSI YANG DIPAKAI MODUL LAIN ====

    std::vector<Asteroid*> scanAllAsteroids(std::function<bool(const Asteroid&)> predicate); // mengambil kumpulan alamat memori asteroid yang di scan sesuai dengan lamda function
    Asteroid *scanAsteroid(std::function<bool(const Asteroid&)> predicate); // mengambil satu alamat memori asteorid yang memenuhi kondisi lambda function

    void update(float deltaTime);   // handle logika game
    void draw();                    // handle visual game   

    // ==== DATA DAN FUNGSI HELPER ====

private:

    DifficultyManager difficultyManager;

    // - ASTEROID POOL

    Asteroid poolAsteroid[50];
    int asteroidCurrent = 0;            // dipakai sebagai index poolAsteroid untuk pergantian spawn asteroid yang ada di pool
    void spawnPoolAsteroid(int diff);   // digunakan untuk mengaktifkan asteroid yang ada di pool (ketika diaktifkan asteroid langsung berjalan di modul asteroid)
    // - EVENT ASTEROID SHOWER

    SinglyLinkedList<Asteroid> asteroidShower;
    void addShowerNode();       // menambahkan node 
    void triggerShowerWave();   // mulai wave shower (reset cursor + start timer)
    void updateShowerWave(float deltaTime); // proses wave per-frame, aktifkan asteroid satu per satu

    // - TIMER TIMING EKSEKUSI

    Timer timerNormal;          // untuk spawn (mengaktifkan) asteroid normal di pool
    Timer timerShower;          // untuk eksekusi 
    Timer timerShowerInterval;  // untuk eksekusi interval tiap node asteroid
    Timer timerExecute;         // untuk interval eksekusi per node event
    Timer timerAddNode;         // untuk interval penambahan node shower (60 detik)
    bool showerWaveActive = false; // apakah wave shower sedang berjalan

    // - QUEUE EKSEKUSI EVENT

    std::priority_queue<EventType> eventQueue;  
    void executeEvent();    // eksekusi event sesuai prioritas
};
